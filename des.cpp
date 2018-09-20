#include <iostream>
#include <bitset>
#include <fstream>

int getBitAtN(int bits, int n){
	return std::bitset<10>(bits)[n];
}
int permute(int binary, int permutation[], int size, int gre){
	int result = 0;
	for(int i = 0; i < size; i++){
		result = (result << 1) | getBitAtN(binary, gre - permutation[i]);
	}
	return result;
}

void swap(int &a, int &b){
	int c = b;
	b = a;
	a = c;
}

//left shift 5-bit number once to the left
int circLeftShift(int binary){
	return 0b11111 & ((binary << 1) | binary >> 4);
}

int leftShiftKey(int key){
	//split left and right
	int right = key & 0b11111;
	int left = key >> 5;

	//perform circular left shifts
	right = circLeftShift(right);
	left = circLeftShift(left);

	int new_key = (left << 5) | right;
	return new_key;
}

int ffunction(int right, int key){
	//S0 substitution matrix
	int S0[4][4] = {{1, 0, 3, 2},
				    {3, 2, 1, 0},
				    {0, 2, 1, 3},
				    {3, 1, 3, 2}};
	//S1 subtitution matrix
	int S1[4][4] = {{0, 1, 2, 3},
				    {2, 0, 1, 3},
				    {3, 0, 1, 0},
				    {2, 1, 0, 3}};

	int innerBits[2] = {2, 3};
	int outerBits[2] = {1, 4};
	//Expansion/Permutation
    int expan[8] = {4, 1, 2, 3, 2, 3, 4, 1};

    //P4
	int P4[4] = {2, 4, 3, 1};

	//perform the expansion
	int bits = permute(right, expan, 8, 4);
	
	//add the bits and key
	bits = bits ^ key;
	//split into right and left
	int right2 = bits & 0b1111;
	int left2 = bits >> 4;



	//lookup in the S-tables and permute
	return permute(
		(S0[permute(left2, outerBits, 2, 4)][permute(left2, innerBits, 2, 4)] << 2) |
		S1[permute(right2, outerBits, 2, 4)][permute(right2, innerBits, 2, 4)],
		P4, 4, 4);

}
//plaintext should be 8 bits, key should be 10 bits
char encrypt(int plaintext, int key){
	//initial permutation of bit positions
	int initial_perm_pt[8] = {2, 6, 3, 1, 4, 8, 5, 7};
	//inverse permutation of bit positions
	int inverse_initial_perm[8] = {4, 1, 3, 5, 7, 2, 8, 6};
	//P10: Permutation of 10-bit
	int P10[10] = {3, 5, 2, 7, 4, 10, 1, 9, 8, 6};
	//P8: Permutation of 8-bit
	int P8[8] = {6, 3, 7, 4, 8, 5, 10, 9};
	
	//perform initial permutation
	int permuted_plaintext = permute(plaintext, initial_perm_pt, 8, 8);
	//seperate left and right
	//bitmask 1111 to get right digits
	int right = permuted_plaintext & 0b1111;
	//bitshift right 4 times to get left digits
	int left = permuted_plaintext >> 4;

	//Perform P10 on key
	key = permute(key, P10, 10, 10);
	

	//get the key after circular leftshifts
	key = leftShiftKey(key);
	
	//first ffunction	
	left = left ^ ffunction(right, permute(key, P8, 8, 10));

	//swap left and right
	swap(left, right);
	//do it again
	key = leftShiftKey(key);

	left = left ^ ffunction(right, permute(key, P8, 8, 10));

	plaintext = (left << 4) | right;
	return (permute(plaintext, inverse_initial_perm, 8, 8));
}

//take arguements as <input file> <output file>
int main(int argc, char** argv){
	std::string infile_name = argv[1];
	std::string outfile_name = argv[2];
	int key = 0b0010101111;

	//open the files to input and output
	std::ifstream infile;
	std::ofstream outfile;
	infile.open(infile_name);
	outfile.open(outfile_name);

	//consider it a character
	char temp;
	while(infile.get(temp)){
		//encrypt byte by byte
		temp = encrypt(temp, key);
		//output to a file
		outfile.put(temp);
	}
}