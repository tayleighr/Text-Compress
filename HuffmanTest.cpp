#include "HuffmanTree.h"
#include <iostream>

using namespace std;

int main()
{
	HuffmanTree a;

	a.makeCompressedFile("thisText.txt", "encoded.txt");

	cin.ignore();
	return 0;
}