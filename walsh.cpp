#include <iostream>
#include <iomanip>
#include <cstring>

using namespace std;

int f[1024][1024];
int spectrA[1024][1024], restoreA[1024][1024]; 
int spectrO[1024][1024], restoreO[1024][1024];

int matrix[1024][1024];
int w[1024][1024];

void generateWalsh(int n)
{
    for (int x = 0; x < n; x++)
        w[0][x] = 1;

    for (int i = 1; i < n; i++)
    {
        for (int x = 0; x < n/2; x++)
            w[i][x] = w[i/2][2*x];

        //even i
        if (i % 2 == 0)
        {
            for (int x = n/2; x < n; x++)
                w[i][x] = w[i][x-n/2];
        }
        //odd i
        else
        {
            for (int x = n/2; x < n; x++)
                w[i][x] = - w[i][x-n/2];
        }
    }
}

int generateAgregate(int m, int x1, int x2)
{
	memset(matrix, 0, 1024*1024*sizeof(int));
	for (int k1 = 0; k1 <= m; k1++)
	{
		int k2 = m - k1;
		for (int r1 = 0; r1 < (1 << k1); r1++)
		{
			int i1 = r1 * (1 << (m - k1));
			for (int r2 = 0; r2 < (1 << k2); r2++)
			{
				int i2 = r2 * (1 << (m - k2));
				int pwr = (r1 + (int)(k1 > 0)) % 2;

				if (pwr == 0)
					matrix[x1^i1][x2^i2]++;
				else
					matrix[x1^i1][x2^i2]--;
			}
		}
	}
}

int getFourierCoefficientOriginal(int n, int (*src)[1024], int i1, int i2)
{
	int ans = 0;
	for (int r1 = 0; r1 < n; r1++)
		for (int r2 = 0; r2 < n; r2++)
			ans += src[r1][r2] * w[i1][r1] * w[i2][r2];
	return ans;
}

void makeFourierOriginal(int n, int (*src)[1024], int (*dst)[1024])
{
	for (int i1 = 0; i1 < n; i1++)
		for (int i2 = 0; i2 < n; i2++)
			dst[i1][i2] = getFourierCoefficientOriginal(n, src, i1, i2);
}

int getFourierCoefficientAgregate(int n, int (*src)[1024], int i1, int i2)
{
	int ans = 0;
	for (int r1 = 0; r1 < n; r1++)
		for (int r2 = 0; r2 < n; r2++)
			ans += f[r1][r2] * w[i1][r1] * w[i2][r2] * matrix[r1][r2];
	return ans;
}


void makeFourierAgregate(int n, int (*src)[1024], int (*dst)[1024])
{
	for (int i1 = 0; i1 < n; i1++)
		for (int i2 = 0; i2 < n; i2++)
			dst[i1][i2] = getFourierCoefficientAgregate(n, src, i1, i2);
}

void print(int n, int (*matr)[1024], const char *head)
{
	cout << head << ":" << endl;
	int sum = 0;
	for (int i1 = 0; i1 < n; i1++)
	{
		for (int i2 = 0; i2 < n; i2++)
		{
			if (matr[i1][i2] == 0)
				cout << setw(4) << (char)26;
			else
				cout << setw(4) << matr[i1][i2];
			sum += matr[i1][i2];
		}
		cout << endl;
	}
	cout << sum << endl;
}

void scan(int n)
{
	for (int i1 = 0; i1 < n; i1++)
		for (int i2 = 0; i2 < n; i2++)
			cin >> f[i1][i2];
}
int main()
{
	int n, m;

	cin >> m;
	n = 1 << m;

	generateWalsh(n);
	print(n, w, "Walsh");

	generateAgregate(m, 0, 0);
	print(n, matrix, "Agregate");

	scan(n);
	print(n, f, "Source");

	makeFourierOriginal(n, f, spectrO);
	print(n, spectrO, "Spectr Original");
	
	makeFourierAgregate(n, f, spectrA);
	print(n, spectrA, "Spectr Agregate");
	return 0;
}

