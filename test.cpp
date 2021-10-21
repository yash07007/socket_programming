// reading a text file
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
// #include <vector>
// #include <functional>

using namespace std;

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

// template<typename Map>

// void print_map(Map& m)
// {
// 	cout << "[ ";
//     for (auto &item : m) {
//         cout << item.first << ":" << item.second << " ";
//     }
//     cout << "]\n";
// }


int main() {

	// string line;
  	// ifstream myfile ("list.txt");
  	// int isState = 1;
  	// string stateName = "";
  	// map<string, bool> LocationMap;
	// LocationMap.insert(pair<string, int>("Yash", true));
	// LocationMap.insert(pair<string, int>("Vyom", true));
	// LocationMap["Yash"] = "99";
	// print_map(LocationMap);
	
	// string key;
	// cin >> key;
	// while(key != "exit") {
	// 	if (LocationMap[key])
	// 	{
	// 		cout << "Yes" << endl;
	// 		print_map(LocationMap);
	// 	}
	// 	else {
	// 		cout << "No" << endl;
	// 		print_map(LocationMap);
	// 	}
	// 	cin >> key;
	// }

	
      // COMPUTATION

    //Reading and storing data in MAP from list.txt

    // // initalising Maps (Dictionary)
  	// map<string, string> LocationMap;
  	// map<string, bool> CityPresent;

    // // opening file in read mode
  	// ifstream myfile("list.txt");
    
    // if(myfile.is_open())
  	// {
  	//     string line,stateName;
  	//     int isState = 1;

    // 	while(getline(myfile,line))
    // 	{
    //         // If state store it in state variable
    //   		if(isState == 1){
    //             stateName = line;
    //             cout << stateName << endl;
    //             isState = 0;
    //   	    }
    //   	    else {
    //             size_t pos = 0;
    //             string city;
    //             while ((pos = line.find(',')) != string::npos) {
    //                 city = line.substr(0, pos);
    //                 if(!CityPresent[city]) {
	// 					CityPresent.erase(city);
    //                     cout << city << endl;
    //                 }
    //                 CityPresent.insert(make_pair(city, true));
    //                 LocationMap.insert(make_pair(city, stateName));
    //                 line.erase(0, pos + 1);
    //             }
    //             LocationMap.insert(make_pair(line, stateName));
    //             isState = 1;
    //   	    }
    //     }
    //     myfile.close();
	// 	cout << "Main server has read the state list from list.txt" << endl;
    // }
  	// else {
	// 	cout << "Unable to open list.txt file" << endl;
    //     exit(1); 
	// } 

	int a = 677;
	string b = "popat";

	cout << patch::to_string(a) + b; 

  	return 0;
}




