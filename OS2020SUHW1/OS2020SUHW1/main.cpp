#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include <pthread.h>
using namespace std;
unordered_map<int, int> m2;

struct character
{
    char symbol;
    int frequency;
    string code;
};

void sortCharacterByFrequency(character arr[], unsigned long size)
{
    bool swapped;
    do
    {
        swapped = false;
        for(int x = 0; x < size - 1; x++)
        {
            if(arr[x].frequency < arr[x + 1].frequency)
            {
                swap(arr[x].frequency, arr[x + 1].frequency);
                swap(arr[x].symbol, arr[x + 1].symbol);
                swapped = true;
            }
        }
    } while(swapped);
}

void sortCharacterByFrequencyThenBySymbol(character arr[], unsigned long size)
{
    for(int x = 0; x < size - 1; x++)
    {
        int f = arr[x].frequency;
        int y = x;
        while(arr[y + 1].frequency == f && (y + 1) < size)
        {
            y++;
        }
        
        bool swapped;
        do
        {
            swapped = false;
            for(int b = x; b < y; b++)
            {
                if(arr[b].symbol < arr[b + 1].symbol)
                {
                    swap(arr[b].symbol, arr[b + 1].symbol);
                    swapped = true;
                }
            }
        } while(swapped);
        x = y;
    }
}

void *decimalToBinary(void *arg)
{
    struct character *pkt = (struct character *)arg;
    int n = stoi(pkt->code);
    string b;
    for(int i = 0; n > 0; i++)
    {
        b += to_string(n % 2);
        n = n / 2;
    }
    
    for (int i = 0; i < b.length() / 2; i++)
    {
        swap(b[i], b[b.length() - i - 1]);
    }
    pkt->code = b;
    return NULL;
}


string removeDuplicateCharacters(string s)
{
    for(int i = 0; i < s.length(); i++)
    {
        char currChar = s[i]; //holds current character
        for(int j = i + 1; j < s.length(); j++)
        {
            if(currChar == s[j])
                s.erase(remove(s.begin() + j, s.end(), s[j]), s.end());
        }
    }
    return s;
}

int main(int argc, char* argv[1])
{
    //get txt file from argv
    ifstream in;
    in.open(argv[1]);
    if(in.fail())
    {
        cout << "file not found" << endl;
        exit(1);
    }
    
    //storing the whole txt file into string "file"
    //symbol
    string file;
    string line;
    while(getline(in, line))
    {
        file.append(line);
    }
    in.close();
    
    //freqeuncy
    string characters = file;
    
    //removes duplicate characters from the file while maintaining original order
    file = removeDuplicateCharacters(file);
    
    character c[file.length()];
    
    //symbol
    //setting symbol
    for(int x = 0; x < file.length(); x++)
    {
        c[x].symbol = file[x];
    }
    
    //frequency
    //setting frequency
    for(int x = 0; x < file.length(); x++)
    {
        c[x].frequency = 0;
        for(int y = 0; y < characters.length(); y++)
        {
            if(c[x].symbol == characters[y])
            {
                c[x].frequency = c[x].frequency + 1;
            }
        }
    }
    
    //code
    character temp[file.length()];
    for(int x = 0; x < file.length(); x++)
    {
        temp[x].symbol = c[x].symbol;
        temp[x].frequency = c[x].frequency;
    }
    
    sortCharacterByFrequency(temp, file.length());
    sortCharacterByFrequencyThenBySymbol(temp, file.length());
    
    //pthread
    #define NTHREADS file.size()
    pthread_t tid[NTHREADS];
    
    for(int x = 0; x < NTHREADS; x++)
    {
        for(int y = 0; y < NTHREADS; y++)
        {
            if(c[y].frequency == temp[x].frequency && c[y].symbol == temp[x].symbol)
            {
                c[y].code = to_string(x);
                if(pthread_create(&tid[x], NULL, decimalToBinary, &c[y]))
                {
                    fprintf(stderr, "Error creating thread\n");
                    return 1;
                }
            }
        }
    }
    
    for(int x = 0; x < NTHREADS; x++)
    {
        pthread_join(tid[x], NULL);
    }
    
    //output
    for(int x = 0; x < file.length(); x++)
    {
        switch(c[x].symbol)
        {
            case char(13):
                cout << "Symbol: <EOL>, Frequency: " << c[x].frequency << ", Code: " << setfill('0') << setw(ceil(log2(file.length()))) << c[x].code << endl;
                break;
                
            case ' ':
                cout << "Symbol: , Frequency: " << c[x].frequency << ", Code: " << setfill('0') << setw(ceil(log2(file.length()))) << c[x].code << endl;
                break;
                
            default:
                cout << "Symbol: " << c[x].symbol << ", Frequency: " << c[x].frequency << ", Code: " << setfill('0') << setw(ceil(log2(file.length()))) << c[x].code << endl;
                break;
        }
    }
    return 0;
}
