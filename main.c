#include <windows.h>
#include <stdio.h>

// Variable global para la layer
int layer = 1;
int intervalTimeDobleSpace = 200;
DWORD lastSpacePressTime = 0;

// Estructura para mapear teclas en capa 2 a sus códigos virtuales
const struct
{
    int original;
    int remap;
} remapDictionary[] = {
    {0x41, VK_LEFT},   // A
    {0x53, VK_DOWN},   // S
    {0x44, VK_UP},     // D
    {0x46, VK_RIGHT},  // F
    {0x4A, VK_HOME},   // J
    {0x4B, VK_END},    // K
    {0x4C, VK_RETURN}, // L
    {0xBA, VK_BACK},   // Ñ (Acento grave - Tecla de consola en español)
    {0x08, VK_BACK}    // Backspace
};

// Función para remapear la tecla según el diccionario
int remapKey(char originalKey)
{
    for (int i = 0; i < sizeof(remapDictionary) / sizeof(remapDictionary[0]); ++i)
    {
        if (originalKey == remapDictionary[i].original)
        {
            return remapDictionary[i].remap;
        }
    }
    return 0; // Tecla no encontrada en el diccionario
}

void simulationKeyEvent(int keyCode)
{
    keybd_event(keyCode, 0, 0, 0);
    keybd_event(keyCode, 0, KEYEVENTF_KEYUP, 0);
}

int changeCurrentLayer()
{
    DWORD currentTime = GetTickCount();
    // Si la última pulsación de tecla espacio fue dentro de los últimos 200 milisegundos
    if (currentTime - lastSpacePressTime < intervalTimeDobleSpace)
    {
        if (layer == 1)
        {
            layer = 2;
            simulationKeyEvent(VK_BACK);
            printf("Cambiar a layer %d\n", layer);
            return 1;
        }
        else if (layer == 2)
        {
            layer = 1;
            simulationKeyEvent(VK_BACK);
            printf("Cambiar a layer %d\n", layer);
            return 1;
        }
    }
    lastSpacePressTime = currentTime;
    return 0;
}

// Función de callback para manejar los eventos del teclado
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        KBDLLHOOKSTRUCT *kbStruct = (KBDLLHOOKSTRUCT *)lParam;

        if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) && kbStruct->vkCode == VK_SPACE)
        {
            if (changeCurrentLayer() == 1)
                return 1;
        }
        else if (layer == 2 && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN))
        {
            printf("Tecla original: %d\n", kbStruct->vkCode);
            int remappedKeyCode = remapKey(kbStruct->vkCode);
            printf("Tecla remapeada: %d\n", remappedKeyCode);
            if (remappedKeyCode != 0)
            {
                // Simular el evento de la tecla remapeada
                keybd_event(remappedKeyCode, 0, 0, 0);
                keybd_event(remappedKeyCode, 0, KEYEVENTF_KEYUP, 0);
                printf("Tecla remapeada: %d\n", remappedKeyCode);
                // Aquí puedes realizar la acción correspondiente al evento remapeado
                return 1;
            }
            // Simular el evento de la tecla 'B'
            // keybd_event('B', 0, 0, 0);
            // keybd_event('B', 0, KEYEVENTF_KEYUP, 0);
            // Cancelar el evento original de la tecla 'K'
        }
    }
    // Dejar que otros programas manejen el evento de teclado
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main()
{
    // Instalar el hook del teclado
    HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    // Loop para mantener el programa en ejecución
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    // Liberar el hook del teclado antes de salir
    UnhookWindowsHookEx(keyboardHook);
    return 0;
}
