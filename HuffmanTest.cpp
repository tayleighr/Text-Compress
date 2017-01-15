#include "HuffmanTree.h"
#include <iostream>

using namespace std;

int main()
{
	HuffmanTree a;

	a.makeCompressedFile("thisText.txt", "encoded.txt");
	a.decompressFile("encoded.txt", "output.txt");
	cin.ignore();
	return 0;
}