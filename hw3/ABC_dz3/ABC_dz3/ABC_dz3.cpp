#include<iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <thread>
#include <iomanip>
using namespace std;
/// <summary>
/// prints matrix, decimal number to 3 nums after point
/// </summary>
/// <param name="m"></param>
void print_matrix(vector< vector<double> > m)
{
    int dim = m.size();
    for (int i = 0;i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            cout << m[i][j] << setprecision(3) << " ";
        }
        cout << "\n";
    }
}
/// <summary>
/// creates matrix out of numbers 0 to 9
/// </summary>
/// <param name="dim"></param>
/// <returns></returns>
vector< vector<double> > create_matrix(int dim)
{   
    vector< vector<double> > matrix(dim, vector<double>(dim));
    for (int i = 0;i < dim;i++)
        for (int j = 0; j < dim; j++)
        {
            matrix[i][j] = (rand() % 10);
        }
    return matrix;
}
/// <summary>
/// calculates determinant of the matrix
/// </summary>
/// <param name="mat2"></param>
/// <param name="s"></param>
/// <returns></returns>
int determinant(vector<vector<double> > mat2, int s)
{
    if (s == 1) return mat2[0][0];
    if (s == 2) {

        return mat2[0][0] * mat2[1][1] -
            mat2[0][1] * mat2[1][0];
    }
    else {
        vector<vector<double> > mat1(s - 1),
            mat3(s - 1), mat4(s - 1);
        int k, l, m, i, j;

        for (i = 0; i < s - 1; i++) {
            mat1[i] = vector<double>(s - 1);
            mat3[i] = vector<double>(s - 1);
            mat4[i] = vector<double>(s - 1);
        }

        for (i = 1; i < s; i++) {
            k = 0;
            l = 0;
            m = 0;
            for (j = 0; j < s; j++) {
                if (j != 0) {
                    mat1[i - 1][k] = mat2[i][j];
                    k++;
                }
                if (j != 1) {
                    mat3[i - 1][l] = mat2[i][j];
                    l++;
                }
                if (j != 2) {
                    mat4[i - 1][m] = mat2[i][j];
                    m++;
                }
            }
        }

        return mat2[0][0] * determinant(mat1, s - 1) -
            mat2[0][1] * determinant(mat3, s - 1) +
            mat2[0][2] * determinant(mat4, s - 1);
    }
}
/// <summary>
/// calculates cofactor of the element m[p,q] there m - matrix, p,q - coordinates
/// </summary>
/// <param name="m"></param>
/// <param name="p"></param>
/// <param name="q"></param>
/// <returns></returns>
int calculateCofactor(vector< vector<double> > m, int p, int q) {
    int i = 0, j = 0;
    int n = m.size();
    vector< vector<double> > temp(n-1, vector<double>(n-1));
    for (int row = 0; row < n; row++)
    {
        for (int col = 0; col < n; col++)
        {
            if (row != p && col != q)
            {
                int a = m[row][col];
                temp[i][j] = a;
                j++;

                if (j == n - 1)
                {
                    j = 0;
                    i++;
                }
            }
        }
    }
    int sign = ((p + q) % 2 == 0) ? 1 : -1;
    return sign * determinant(temp,n-1);
}
/// <summary>
/// assignes elements, for which cofactors need to ne calculated to the thread
/// </summary>
/// <param name="res"></param>
/// <param name="input"></param>
/// <param name="thread_num"></param>
/// <param name="thread_count"></param>
/// <param name="det"></param>
/// <returns></returns>
void* func(vector< vector<double> >& res, vector< vector<double> > input, int thread_num, int thread_count, double det) {
    int dim = input.size();
    for (unsigned int i = thread_num; i < dim*dim; i += thread_count) {
        res[i / dim][i % dim] = calculateCofactor(input, i / dim, i % dim) / det;
    }

    return nullptr;
}
/// <summary>
/// inverses a matrix
/// </summary>
/// <param name="m"></param>
/// <param name="thread_count"></param>
/// <param name="det"></param>
/// <returns></returns>
vector< vector<double> > inverse(vector< vector<double> > m, int thread_count, double det){
    int dim = m.size();
    vector< vector<double> > inverse(dim, vector<double>(dim));
    vector<thread> threads;

    for (int i = 0; i < thread_count; i++) {
        threads.push_back(thread(func, ref(inverse),m,i,thread_count,det));
    }
    for (thread& th : threads)
    {
        if (th.joinable())
            th.join();
    }

    return inverse;
}




int get_dim()
{
    int dim;
    cout << "Input n to create n*n matrix:";
    cin >> dim;
    if (dim < 2)
    {
        cout << "n cannot be less than 2. ";
        return get_dim();
    }
    else
        return dim;
}

int get_threads_count()
{
    int threads_count;
    cout << "Input how many threads you want to use:";
    cin >> threads_count;
    if (threads_count < 1)
    {
        cout << "thread count cannot be less than 1. ";
        return get_threads_count();
    }
    else
        return threads_count;
}

int main()
{
    int dim = get_dim();
    int thread_count = get_threads_count();
    vector< vector<double> > matrix = create_matrix(dim);
    
    cout << "generated matrix:\n";
    print_matrix(matrix);
    int det = determinant(matrix, dim);
    if (det != 0)
    {  
        vector< vector<double> > result_matrix = inverse(matrix, thread_count,det);
        cout << "inverse:\n";
        print_matrix(result_matrix);
    }
    else 
    {
        cout << "Determinate equals 0, cannot inverse function";
    }
}