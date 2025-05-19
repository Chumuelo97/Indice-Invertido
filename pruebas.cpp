/*#include <string>
#include <iostream>

static bool letra2Ascii(unsigned char c)
{
    //return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c > 0xC0 && c < 0xFF); //<-- va a ayudar a considerar letras con tildes y ñ-Ñ i wanna dai
    int codigo;
    unsigned char letra = c;

    codigo = letra;

    return codigo;
}

int main(){
    std::string palabra = "Simulación";
    for (unsigned char c : palabra)
    {
        if (letra2Ascii(c))
        {
            std::cout << c;
        }
    }
    std::cout << std::endl;

    return 0;
}*/