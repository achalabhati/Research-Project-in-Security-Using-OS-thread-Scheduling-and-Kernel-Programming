// A Dynamic Programming based C++ program to find minimum
// number operations to convert str1 to str2
#include<bits/stdc++.h>
using namespace std;

// Utility function to find minimum of three numbers
int min(int x, int y, int z) 
{
	return min(min(x, y), z);
}

int editDistDP(string str1, string str2, int m, int n)
{
	// Create a table to store results of subproblems
	int dp[m+1][n+1];

	// Fill d[][] in bottom up manner
	for (int i=0; i<=m; i++)
	{
		for (int j=0; j<=n; j++)
		{
			// If first string is empty, only option is to
			// isnert all characters of second string
			if (i==0)
				dp[i][j] = j; // Min. operations = j

			// If second string is empty, only option is to
			// remove all characters of second string
			else if (j==0)
				dp[i][j] = i; // Min. operations = i

			// If last characters are same, ignore last char
			// and recur for remaining string
			else if (str1[i-1] == str2[j-1])
				dp[i][j] = dp[i-1][j-1];

			// If last character are different, consider all
			// possibilities and find minimum
			else
				dp[i][j] = 1 + min(dp[i][j-1], // Insert
								dp[i-1][j], // Remove
								dp[i-1][j-1]); // Replace
		}
	}

	return dp[m][n];
}

// Driver program
int main(int argc, char *argv[])
{
	// your code goes here
	
	string str1;
	str1=argv[1]; // experimental sequence
	string str2 ;
	str2=argv[2]; //original sequence
	
	int err = editDistDP(str1, str2, str1.length(), str2.length());
	
	float res = ((str2.length()-err)/(float)(str2.length())) * 100;
	printf("%f",res);

	return 0;
}
