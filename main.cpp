#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "LabeledTree/LabeledTree.hpp"
#include "XBWT/XBWT.hpp"

using namespace std;

int main() {
    // Stringa di input
    std::string str = "(A(B(D(a))(a)(E(b)))(C(D(c))(b)(D(c)))(B(D(b))))";
    cout << "Input string: " << str << endl;

    // Mappa per convertire le lettere in interi
    std::map<char, unsigned int> letterToInt;
    unsigned int currentInt = 1;

    // Popola la mappa con le lettere presenti nella stringa
    for (char ch : str) {
        if (isalpha(ch) && letterToInt.find(ch) == letterToInt.end()) {
            letterToInt[ch] = currentInt++;
        }
    }

    // Stampa la mappa per verificare la conversione
    for (const auto& pair : letterToInt) {
        std::cout << pair.first << " -> " << pair.second << std::endl;
    }

    // Converti la stringa utilizzando la mappa
    std::string convertedStr;
    for (char ch : str) {
        if (isalpha(ch)) {
            convertedStr += std::to_string(letterToInt[ch]);
        } else {
            convertedStr += ch;
        }
    }

    // Stampa la stringa convertita
    std::cout << "Converted string: " << convertedStr << std::endl;

    // Crea l'albero etichettato utilizzando la stringa convertita
    LabeledTree<unsigned int> tree(convertedStr);
    cout << "Tree: " << tree.toString() << endl;

    // build an XBWT
    XBWT xbwt(tree);

    return 0;
}