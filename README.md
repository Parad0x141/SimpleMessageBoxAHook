# MessageBoxA Hook Demo 🎣

**Demonstration concrète de hooking de fonction Windows**  
*Exemple réel utilisé dans l'article "Hooking 101 Part 2" + showcase de BytesForge*

[![C++](https://img.shields.io/badge/C++-20+-blue)](https://en.cppreference.com/)
[![Windows](https://img.shields.io/badge/Platform-Windows-lightgrey)](https://windows.com)
[![BytesForge](https://img.shields.io/badge/Powered%20by-BytesForge-orange)](https://github.com/Parad0x141/BytesForge)

## 📖 Contexte

Ce code a été développé pour l'article **"Hooking 101 - Partie 2 : Démonstration Live"** illustrant les techniques de hooking de fonctions Windows. Il sert également d'exemple d'utilisation concret de la librairie **BytesForge**.

## 🎯 Ce que fait cette démo

- **Intercepte** tous les appels à `MessageBoxA`
- **Modifie** le texte et le titre des boîtes de dialogue
- **Montre** le hooking en action avec restauration automatique
- **Utilise** BytesForge pour générer le shellcode de redirection

## 🚀 Comment ça marche

### 1. Installation du Hook

```cpp
// Sauvegarde des bytes originaux
ReadProcessMemory(GetCurrentProcess(), originalMessageBoxA, stolenBytes, 12, nullptr);

// Génération du hook avec BytesForge
X64Assembler hook;
hook.MovRax((uint64_t)HookedMessageBox);  // MOV RAX, notre fonction
hook.JmpRax();                            // JMP RAX

// Écriture du hook
WriteProcessMemory(GetCurrentProcess(), originalMessageBoxA, hook.GetBytes().data(), 12, nullptr);
```
### 2. Fonction de Remplacement

```cpp
int WINAPI HookedMessageBox(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
    // Restauration des bytes originaux
    WriteProcessMemory(GetCurrentProcess(), originalMessageBoxA, stolenBytes, 12, nullptr);
    
    // Modification des arguments
    lpText = "Hooked by Parad0x141 !";
    lpCaption = "Hello World !";
    
    // Appel de l'original avec nos arguments
    return MessageBoxA(hWnd, lpText, lpCaption, uType);
}
```
🛠️ Compilation

```cpp
# Avec Visual Studio
cl.exe /EHsc /std:c++20 demo.cpp BytesForge.cpp

# Avec GCC/MinGW
g++ -std=c++20 -static demo.cpp BytesForge.cpp -luser32 -o hook_demo.exe
}
```
🎪 Démonstration
Le programme montre trois phases :

Avant hook : Comportement normal

Pendant hook : Texte modifié → "Hooked by Parad0x141 !"

Après hook : Restauration automatique

🔧 Dépendances

BytesForge : Librairie de génération de shellcode

Windows API : user32.dll pour MessageBoxA

C++20 : Compilateur moderne requis

📚 Pour aller plus loin

Ce code démontre :

Hooking bas niveau avec modification d'opcodes

Gestion mémoire avec VirtualProtect/WriteProcessMemory

Génération de code avec BytesForge

Restauration sécurisée pour éviter les boucles infinies

⚠️ Avertissement
À des fins éducatives uniquement. Testez uniquement sur vos propres systèmes.

🔗 Liens
Article Hooking 101 -> https://www.linkedin.com/pulse/functions-hooking-101-part-2-code-trace-d%25C3%25A9fonce-cyril-bouvier-ppxle/?trackingId=UZ7ysVgfTMiD5qbseddG8Q%3D%3D

BytesForge Library -> https://github.com/Parad0x141/BytesForge
