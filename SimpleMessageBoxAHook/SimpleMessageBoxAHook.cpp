#define NOMINMAX
#include <iostream>
#include <Windows.h>

#include "BytesForge.hpp"   

// Pointeur vers l'address de MessageBoxA
FARPROC PointeurVersAddresseMessageBoxA = nullptr; 
// 12 bytes qu'on va voler à MessageBoxA
BYTE    BytesOriginauxDeMessageBoxA[12] = {};  
// out param de WriteProcessMemory
SIZE_T  numDeBytesEcrits = 0;                       


// Notre proxy qui remplace MessageBoxA
extern "C" int WINAPI HookedMessageBox(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
    std::cout << "[HOOK] MessageBoxA interceptee!\n";

    // On dehook pour eviter une boucle infinie et rendre les commandes à la fonction originale
    WriteProcessMemory(GetCurrentProcess(), (LPVOID)PointeurVersAddresseMessageBoxA,
        BytesOriginauxDeMessageBoxA, sizeof(BytesOriginauxDeMessageBoxA), &numDeBytesEcrits);

    // Change le texte et le caption à la volée pour montrer que notre hook a fonctionné
    lpText = "Hooked by Parad0x141 !";
    lpCaption = "Hello World !";

	// On appelle la fonction originale, mais avec nos arguments modifiés
    return MessageBoxA(hWnd, lpText, lpCaption, uType);
}


static void InstallHook()
{
    // On revcupère l'address de la fonction cible dans notre pointeur global
    PointeurVersAddresseMessageBoxA = GetProcAddress(GetModuleHandleA("user32.dll"), "MessageBoxA");
    if (!PointeurVersAddresseMessageBoxA) return;

    std::cout << "Addresse de MessageBoxA -> " << PointeurVersAddresseMessageBoxA << '\n';

    // Doubleword pour le params out de VirtualProtect
    DWORD ProtectionOriginal;

    // On change la protection mémoire pour pouvoir écrire nos bytes de hook
    VirtualProtect(PointeurVersAddresseMessageBoxA, sizeof(BytesOriginauxDeMessageBoxA), PAGE_EXECUTE_READWRITE, &ProtectionOriginal);


    // On sauvegarde les bytes originaux de la fonction pour restauration future
    ReadProcessMemory(GetCurrentProcess(), PointeurVersAddresseMessageBoxA, BytesOriginauxDeMessageBoxA, sizeof(BytesOriginauxDeMessageBoxA), nullptr);

    // On genère le hook et on saute vers notre fonction proxy HookedMessageBox
    X64Assembler hook;


    // 48 B8 xx xx xx xx xx xx xx xx
    // FF E0   /* total = 12 bytes */

    // On met notre hook dans le registre RAX
    hook.MovRax((uint64_t)HookedMessageBox); 

    // On saute vers l'adresse de notre fonction proxy qui est maintenant dans le registre RAX de la stack
    hook.JmpRax();                           
  

    // On ecrit le hook
    WriteProcessMemory(GetCurrentProcess(), PointeurVersAddresseMessageBoxA,
        hook.GetBytes().data(), hook.Size(), &numDeBytesEcrits);

    // On restaure la protection mémoire
    VirtualProtect(PointeurVersAddresseMessageBoxA, sizeof(BytesOriginauxDeMessageBoxA), ProtectionOriginal, &ProtectionOriginal);
    std::cout << "Hook installe!\n\n";
}

static int wmain()
{
    std::cout << "MessageBoxA Hooking Demo\n";

    // Execution normale
    MessageBoxA(nullptr, "avant le hook", "Hello, je suis la func original !", MB_OK); 

    // Installation de notre hook
    InstallHook();

	// hooké + restauration, on ne verra jamais ce message
    MessageBoxA(nullptr, "Message qu'on ne verra jamais, la fonction est hookée !", "Idem", MB_OK);   

    // Re execution normale apres avoir rendu les stolen bytes
    MessageBoxA(nullptr, "Je suis de nouveau la fonction originale !", "Après restauration des bytes", MB_OK); 
    return 0;
}