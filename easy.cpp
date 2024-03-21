//C0F4DFE8B340D81183C208F70F9D2D797908754D
#include<iostream>
#include<vector>
#include<map>
#include<unordered_map> 
#include<algorithm>
#include<getopt.h>
#include"TableEntry.h"
#include"xcode_redirect.hpp"


using namespace std;

class silly{
  public:
    void get_options(int argc, char** argv);

    void read();

  private:
    struct Options{
        bool quiet_mode = false;
    };

    Options options;

    void printHelp(char** argv){
        cout << "Usage: " << argv[0] << " -h | -q |\n";
        cout << "You want help?  This program accepts:\n";
        cout << "  -q, --quiet   Generate the quiet output \n";
        cout << "  -h, --help    This help page\n";
        exit(0);
    }
    
    template<typename Iterator>
    void print_where_helper(const TableEntry &t, const size_t col, const string opea, 
                            const Iterator &it, const vector<size_t> &index);
    template<typename Iterator>
    void generate_helper(string &index, size_t col_index, Iterator &it) const;

    void create();
    void insert();
    void remove();
    void print();
    void delete_sl();
    void join();
    void generate();
    

    class delete_functor_equal{
      private:
        size_t col_index;
        TableEntry value;
      public:
        delete_functor_equal(const size_t col, const TableEntry& v)
        :col_index(col),value(v){}

        bool operator()(vector<TableEntry> &v) const{
          return v[col_index] == value;
        }
    };


    class delete_functor_less{
      private:
        size_t col_index;
        TableEntry value;
      public:
        delete_functor_less(const size_t col, const TableEntry& v)
        :col_index(col),value(v){}

        bool operator()(vector<TableEntry> &v) const{
          return v[col_index] < value;
        }

    };

    class delete_functor_greater{
      private:
        size_t col_index;
        TableEntry value;
      public:
        delete_functor_greater(const size_t col, const TableEntry& v)
        :col_index(col),value(v){}

        bool operator()(vector<TableEntry> &v) const{
          return v[col_index] > value;
        }
    };
    
    class table{
      public:
        pair<string,size_t> index;
        unordered_map<string,size_t> header;
        vector<EntryType> col_types;
        unordered_map<TableEntry, vector<size_t> > hash;
        map<TableEntry, vector<size_t> > bst;
        vector<vector<TableEntry> > data;

        //table(unordered_map<string,pair<EntryType,size_t> > header)
        //:col_types(col_t), col_names(col_n){}     
        table(unordered_map<string,size_t> &h, vector<EntryType> &col_t)
        : header(h),col_types(col_t){}
    };

    unordered_map<string, table> dataset;
};

void silly::get_options(int argc, char** argv){
    opterr = false; 
    int choice;
    int index = 0;
    option long_options[] = {
      {"quiet", no_argument, nullptr, 'q'},
      {"help", no_argument, nullptr, 'h'},
      { nullptr, 0, nullptr, '\0'}
    };
    while((choice = getopt_long(argc, argv, "qh", long_options, &index)) != -1){

        switch(choice){
        case 'h':
          printHelp(argv);
          exit(0);
      
        case 'q': {
          options.quiet_mode = true;
          break;
        }

        default:
          cerr << "Unknown command line option\n";
          exit(1);
      } // switch
    } // while
}

void silly::read(){
  string cmd;
  string trash;
  cin >> cmd;
  while(cmd != "QUIT"){
    if (cin.fail()) {
    cerr << "Error: Reading from cin has failed" << endl;
    exit(1);
    } // if

    cout << "% ";

    if(cmd[0] == '#'){
      getline(cin,trash);
    }
    

    else if(cmd == "CREATE"){
      create();
    }

    else if(cmd == "REMOVE"){
      remove();
    }
    else if(cmd == "INSERT"){
      insert();
    }
    else if(cmd == "PRINT"){
      print();
    }
    else if(cmd == "DELETE"){
      delete_sl();
    }
    else if(cmd == "JOIN"){
      join();
    }
    else if(cmd == "GENERATE"){
      generate();
    }
    else{
      //error message
      cout << "Error: unrecognized command\n";
      getline(cin,trash);
    }
    cin >> cmd;
  } // while  
  cout << "% " << "Thanks for being silly!\n";
} // read

  


void silly::create(){
  string name;
  cin >> name;
  auto it = dataset.find(name);
  if(it == dataset.end()){

    size_t n;
    cin >> n;
    vector<EntryType> col_types;
    col_types.reserve(n);
    vector<string> col_names;
    col_names.reserve(n);
    unordered_map<string,size_t> header;
    header.reserve(n);
    string temp_1;
    

    for(size_t i = 0; i < n; ++i){
      cin >> temp_1;
      if(temp_1 == "string"){
        col_types.push_back(EntryType::String);
      }
      else if(temp_1 == "double"){
        col_types.push_back(EntryType::Double);
      }

      else if(temp_1 == "int"){
        col_types.push_back(EntryType::Int);
      }

      else if(temp_1 == "bool"){
        col_types.push_back(EntryType::Bool);
      }

      else{
        cout << "Error: unrecognized column types!\n";
      }
    }

    for(size_t j = 0; j < n; ++j){
      cin >> temp_1;
      col_names.push_back(temp_1);
      auto it = header.find(temp_1);
      if(it == header.end()){
        header.emplace(make_pair(temp_1,j));
      }
      else{
        cout << "Error: dupilicated columns are not allowed\n";
        return;
      }
    }

    //table temp = table(col_types,col_names);
    table temp = table(header,col_types);
    dataset.emplace(name,temp);
    cout << "New table " << name << " with column(s) ";
    for(size_t i = 0 ; i < col_names.size(); ++i){
      cout << col_names[i] << " ";
    }
    cout << "created\n";
  }

  else{
      cout << "Error during CREATE: Cannot create already existing table " << name << "\n";
      string trash;
      getline(cin,trash);
  }
}// create()

void silly::remove(){
  string name;
  cin >> name;
  auto it = dataset.find(name);
  if(it != dataset.end()){
    dataset.erase(it);
    cout << "Table " << name << " deleted\n";
  }
  else{
    cout << "Error during REMOVE: " << name << " does not name a table in the database\n";
  }
}

void silly::insert(){
  string trash_1;
  string name;
  size_t num_rows;
  string trash_2;
  cin >> trash_1 >> name >> num_rows >> trash_2;
  auto it = dataset.find(name);
  if(it != dataset.end()){
    size_t exist_num_row = it->second.data.size();
    size_t new_num_row = exist_num_row + num_rows;
    it->second.data.resize(new_num_row);
    //it->second.data.reserve(new_num_row);
    size_t num_cols = it->second.col_types.size();
    for(size_t i = exist_num_row; i < new_num_row; ++i){
      it->second.data[i].reserve(num_cols);
      //vector<TableEntry> vec;
      //vec.reserve(num_cols);
      for(size_t j = 0; j < num_cols; ++j){
        if(it->second.col_types[j] == EntryType::String){
          string s;
          cin >> s;
          it->second.data[i].emplace_back(s);
          //vec.emplace_back(s);
          TableEntry t{s};

          if(it->second.index.first == "hash" && it->second.index.second == j){
            auto it_s = it->second.hash.find(t);
            if(it_s == it->second.hash.end()){
              vector<size_t> temp;
              temp.push_back(i);
              it->second.hash.insert(make_pair(t,temp));
            }
            else{
              it_s->second.push_back(i);
            }
          }
          if(it->second.index.first == "bst" && it->second.index.second == j){
            auto it_s = it->second.bst.find(t);
            if(it_s == it->second.bst.end()){
              vector<size_t> temp;
              temp.push_back(i);
              it->second.bst.insert(make_pair(t,temp));
            }
            else{
              it_s->second.push_back(i);
            }
          }
        }
        else if(it->second.col_types[j] == EntryType::Double){
          double d;
          cin >> d;
          it->second.data[i].emplace_back(d);
          //vec.emplace_back(d);
          TableEntry t{d};
          if(it->second.index.first == "hash" && it->second.index.second == j){
            auto it_d = it->second.hash.find(t);
            if(it_d == it->second.hash.end()){
              vector<size_t> temp;
              temp.push_back(i);
              it->second.hash.insert(make_pair(t,temp));
            }
            else{
              it_d->second.push_back(i);
            }
          }
          if(it->second.index.first == "bst" && it->second.index.second == j){
            auto it_d = it->second.bst.find(t);
            if(it_d == it->second.bst.end()){
              vector<size_t> temp;
              temp.push_back(i);
              it->second.bst.insert(make_pair(t,temp));
            }
            else{
              it_d->second.push_back(i);
            }
          }
        }
        else if(it->second.col_types[j] == EntryType::Int){
          int n;
          cin >> n;
          it->second.data[i].emplace_back(n);
          //vec.emplace_back(n);
          TableEntry t{n};
          if(it->second.index.first == "hash" && it->second.index.second == j){
            auto it_i = it->second.hash.find(t);
            if(it_i == it->second.hash.end()){
              vector<size_t> temp;
              temp.push_back(i);
              it->second.hash.insert(make_pair(t,temp));
            }
            else{
              it_i->second.push_back(i);
            }
          }
          if(it->second.index.first == "bst" && it->second.index.second == j){
            auto it_i = it->second.bst.find(t);
            if(it_i == it->second.bst.end()){
              vector<size_t> temp;
              temp.push_back(i);
              it->second.bst.insert(make_pair(t,temp));
            }
            else{
              it_i->second.push_back(i);
            }
          }
        }
        else if(it->second.col_types[j] == EntryType::Bool){
          bool b;
          cin >> b;
          it->second.data[i].emplace_back(b);
          //vec.emplace_back(b);
          TableEntry t{b};
          if(it->second.index.first == "hash" && it->second.index.second == j){
            auto it_b = it->second.hash.find(t);
            if(it_b == it->second.hash.end()){
              vector<size_t> temp;
              temp.push_back(i);
              it->second.hash.insert(make_pair(t,temp));
            }
            else{
              it_b->second.push_back(i);
            }
          }
          if(it->second.index.first == "bst" && it->second.index.second == j){
            auto it_b = it->second.bst.find(t);
            if(it_b == it->second.bst.end()){
              vector<size_t> temp;
              temp.push_back(i);
              it->second.bst.insert(make_pair(t,temp));
            }
            else{
              it_b->second.push_back(i);
            }
          }
        }
        else{
          //Unreachable
          assert(false);
        }
      }// End of inner for loop
      //it->second.data.push_back(vec);
    }// End of outer for loop
    //Added <N> rows to <tablename> from position <startN> to <endN>
    cout << "Added " << num_rows << " rows to " << name << " from position " << exist_num_row <<
            " to " << new_num_row-1 << "\n";
  }// End of if

  else{
    cout << "Error during INSERT: " << name << " does not name a table in the database\n";
  }
}//End of insert

void silly::print(){
  string from;
  string name;
  size_t num_cols;
  cin >> from >> name >> num_cols;
  auto it = dataset.find(name);
  if(it != dataset.end()){
    string col_names;
    vector<size_t> index;
    vector<string> h;
    index.reserve(num_cols);
    h.reserve(num_cols);
    for(size_t i = 0; i < num_cols; ++i){
      /*bool found = false;
      cin >> col_names;
      for(size_t j = 0; j < it->second.col_names.size(); ++j){
        if(col_names == it->second.col_names[j]){
          found = true;
          index.push_back(j);
          break;
        }
      }
      if(!found){
        cout << "Error during PRINT: " << col_names << " does not name a column in " << name << "\n";
        string trash;
        getline(cin,trash);
        return;
      }*/
      cin >> col_names;
      auto it_1 = it->second.header.find(col_names);
      if(it_1 != it->second.header.end()){
        index.push_back(it_1->second);
        h.push_back(col_names);
      }
      else{
        cout << "Error during PRINT: " << col_names << " does not name a column in " << name << "\n";
        string trash;
        getline(cin,trash);
        return;
      }
    }//End of for loop for storing col_names index

    string cond;
    cin >> cond;
    if(cond == "ALL"){
      if(!options.quiet_mode){
        // Print the colnames header
        for(size_t i = 0; i < h.size(); ++i){
          cout << h[i] << " ";
        }
        cout << "\n";
        // Print the data
        for(size_t i = 0; i < it->second.data.size(); ++i){   
          for(size_t j = 0; j < index.size(); ++j){
            cout << it->second.data[i][index[j]] << " ";
          }
          cout<< "\n";
        }
      }
      cout << "Printed " << it->second.data.size() << " matching rows from " << name << "\n";
    }// End of PRINT ALL

    else if(cond == "WHERE"){
      string col_name;
      string opea;
      cin >> col_name >> opea;
      size_t col = 0;
      auto it_2 = it->second.header.find(col_name);
      if(it_2 != it->second.header.end()){
        col = it_2->second;
      }
      else{
        cout << "Error during PRINT: " << col_name << " does not name a column in " << name << "\n";
        string trash;
        getline(cin,trash);
        return;
      }
      /*bool found = false;
      for(size_t i = 0; i < it->second.col_names.size(); ++i){
        if(it->second.col_names[i] == col_name){
          col = i;
          found = true;
          break;
        }
      }
       
      if(!found){
        cout << "Error during PRINT: " << col_name << " does not name a column in " << name << "\n";
        string trash;
        getline(cin,trash);
        return;
      }*/
      
      // Print the colnames header
      if(!options.quiet_mode){
        for(size_t i = 0; i < h.size(); ++i){
          cout << h[i] << " ";
        }
        cout << "\n";
      }

      EntryType t = it->second.col_types[col];

      if(t == EntryType::String){
        string value;
        cin >> value;
        TableEntry tt{value};
        print_where_helper(tt, col, opea, it, index);
      }

      else if(t == EntryType::Double){
        double value;
        cin >> value;
        TableEntry tt{value};
        print_where_helper(tt, col, opea, it, index);
      }

      else if(t == EntryType::Int){
        int value;
        cin >> value;
        TableEntry tt{value};
        print_where_helper(tt, col, opea, it, index);
      }

      else if(t == EntryType::Bool){
        bool value;
        cin >> value;
        TableEntry tt{value};
        print_where_helper(tt, col, opea, it, index);
      }

      else{
        // unreachable
        cout << "Error: Unregcognized type\n";
      }
    }//End of PRINT WHERE

    else{
      cout << "SYNTAX ERROR(neither ALL or WHERE)\n";
    } 
  }

  else{
    cout << "Error during PRINT: " << name << " does not name a table in the database\n";
    string trash;
    getline(cin,trash);
    return;
  }
}// End of PRINT

template<typename Iterator>
void silly::print_where_helper(const TableEntry &t, const size_t col, const string opea, 
                               const Iterator &it, const vector<size_t> &index){
  size_t count = 0;
  if(it->second.index.first == "bst" && it->second.index.second == col){
    if(opea == ">"){
      for(auto it_g = it->second.bst.begin(); it_g != it->second.bst.end(); ++it_g){
        if(it_g->first > t){
          if(!options.quiet_mode){
            for(size_t i = 0; i < it_g->second.size(); ++i){
              for(size_t j = 0; j < index.size(); ++j){
                cout << it->second.data[it_g->second[i]][index[j]] << " ";
              }
              cout << "\n"; 
              ++count;
            }
          } // End of the non quiet mode
          else{
            count +=  it_g->second.size();
          }
        }
      }
      cout << "Printed " << count << " matching rows from " << it->first << "\n";
    }

    else if(opea == "<"){
      for(auto it_g = it->second.bst.begin(); it_g != it->second.bst.end(); ++it_g){
        if(it_g->first < t){
          if(!options.quiet_mode){
            for(size_t i = 0; i < it_g->second.size(); ++i){
              for(size_t j = 0; j < index.size(); ++j){
                cout << it->second.data[it_g->second[i]][index[j]] << " ";
              }
              cout << "\n"; 
              ++count;
            }
          } // End of the non quiet mode
          else{
            count +=  it_g->second.size();
          }
        }
      }
      cout << "Printed " << count << " matching rows from " << it->first << "\n";

    }

    else if(opea == "="){
      for(auto it_g = it->second.bst.begin(); it_g != it->second.bst.end(); ++it_g){
        if(it_g->first == t){
          if(!options.quiet_mode){
            for(size_t i = 0; i < it_g->second.size(); ++i){
              for(size_t j = 0; j < index.size(); ++j){
                cout << it->second.data[it_g->second[i]][index[j]] << " ";
              }
              cout << "\n"; 
              ++count;
            }
          } // End of the non quiet mode
          else{
            count +=  it_g->second.size();
          }
        }
      }
      cout << "Printed " << count << " matching rows from " << it->first << "\n";
    }

    else{
      cout << "Error: unrecognized operator.\n";
      return;
    }
  }// End of the print where bst order

  else{
    if(opea == ">"){  
      for(size_t i = 0; i < it->second.data.size(); ++i){
        if(it->second.data[i][col] > t){
          if(!options.quiet_mode){
            for(size_t j = 0; j < index.size(); ++j){
              cout<< it->second.data[i][index[j]] << " ";
            }
            cout << "\n";
          }      
          ++count;
        }  
      } 
      cout << "Printed " << count << " matching rows from " << it->first << "\n";
    }

    else if(opea == "<"){
      for(size_t i = 0; i < it->second.data.size(); ++i){
        if(it->second.data[i][col] < t){
          if(!options.quiet_mode){
            for(size_t j = 0; j < index.size(); ++j){
              cout<< it->second.data[i][index[j]] << " ";
            }
            cout << "\n";
          }     
          ++count;
        }
      }
      cout << "Printed " << count << " matching rows from " << it->first << "\n";
    }

    else if(opea == "="){
      for(size_t i = 0; i < it->second.data.size(); ++i){
        if(it->second.data[i][col] == t){
          if(!options.quiet_mode){
            for(size_t j = 0; j < index.size(); ++j){
              cout<< it->second.data[i][index[j]] << " ";
           }
           cout << "\n";
          }    
          ++count;
        }
      }
      cout << "Printed " << count << " matching rows from " << it->first << "\n";
    }

    else{
      cout << "Error: unrecognized operator.\n";
      return;
    }
  }// if no index 
}// End of the print_all_helper

void silly::delete_sl(){
  string from, name, where, col_name, opea;
  cin >> from >> name >> where >> col_name >> opea;
  auto it = dataset.find(name);
  if(it != dataset.end()){
    size_t col = 0;
    /*bool found = false;
    for(size_t i = 0; i < it->second.col_types.size(); ++i){
      if(it->second.col_names[i] == col_name){
        col = i;
        found = true;
        break;
      }
    }
       
    if(!found){
      cout << "Error during DELETE: " << col_name << " does not name a column in " << name << "\n";
      string trash;
      getline(cin,trash);
      return;
    }*/
    auto it_3 = it->second.header.find(col_name);
    if(it_3 != it->second.header.end()){
      col = it_3->second;
    }
    else{
      cout << "Error during DELETE: " << col_name << " does not name a column in " << name << "\n";
      string trash;
      getline(cin,trash);
      return;
    }

    EntryType t = it->second.col_types[col];

    if(t == EntryType::String){
      size_t old_size = it->second.data.size();
      string value;
      cin >> value;
      TableEntry tt{value};
      if(opea == "="){
        delete_functor_equal pred(col,tt);
        auto new_end = remove_if(it->second.data.begin(), it->second.data.begin() + (int)it->second.data.size(), pred);
        it->second.data.resize((size_t)(new_end - it->second.data.begin()));
      }
      else if(opea == "<"){
        delete_functor_less pred(col,tt);
        auto new_end = remove_if(it->second.data.begin(), it->second.data.begin() + (int)it->second.data.size(), pred);
        it->second.data.resize((size_t)(new_end - it->second.data.begin()));
      }
      else if(opea == ">"){
        delete_functor_greater pred(col,tt);
        auto new_end = remove_if(it->second.data.begin(), it->second.data.begin() + (int)it->second.data.size(), pred);
        it->second.data.resize((size_t)(new_end - it->second.data.begin()));
      }
      else{
        cout << "Error: unrecognized operator!\n";
        assert(false);
      }
      if(it->second.index.first == "hash"){
        string index = "hash";
        size_t col_index = it->second.index.second;
        generate_helper(index,col_index,it);

      }
      if(it->second.index.first == "bst"){
        string index = "bst";
        size_t col_index = it->second.index.second;
        generate_helper(index,col_index,it);
      }

      cout << "Deleted " << old_size - it->second.data.size() << " rows from " << name << "\n";
    }

    else if(t == EntryType::Double){
      size_t old_size = it->second.data.size();
      double value;
      cin >> value;
      TableEntry tt{value};
      if(opea == "="){
        delete_functor_equal pred(col,tt);
        auto new_end = remove_if(it->second.data.begin(), it->second.data.begin() + (int)it->second.data.size(), pred);
        it->second.data.resize((size_t)(new_end - it->second.data.begin()));
      }
      else if(opea == "<"){
        delete_functor_less pred(col,tt);
        auto new_end = remove_if(it->second.data.begin(), it->second.data.begin() + (int)it->second.data.size(), pred);
        it->second.data.resize((size_t)(new_end - it->second.data.begin()));
      }
      else if(opea == ">"){
        delete_functor_greater pred(col,tt);
        auto new_end = remove_if(it->second.data.begin(), it->second.data.begin() + (int)it->second.data.size(), pred);
        it->second.data.resize((size_t)(new_end - it->second.data.begin()));
      }
      else{
        cout << "Error: unrecognized operator!\n";
        assert(false);
      }

      if(it->second.index.first == "hash"){
        string index = "hash";
        size_t col_index = it->second.index.second;
        generate_helper(index,col_index,it);

      }
      if(it->second.index.first == "bst"){
        string index = "bst";
        size_t col_index = it->second.index.second;
        generate_helper(index,col_index,it);
      }

      cout << "Deleted " << old_size - it->second.data.size() << " rows from " << name << "\n";
    }

    else if(t == EntryType::Int){
      size_t old_size = it->second.data.size();
      int value;
      cin >> value;
      TableEntry tt{value};
      if(opea == "="){
        delete_functor_equal pred(col,tt);
        auto new_end = remove_if(it->second.data.begin(), it->second.data.begin() + (int)it->second.data.size(), pred);
        it->second.data.resize((size_t)(new_end - it->second.data.begin()));
      }
      else if(opea == "<"){
        delete_functor_less pred(col,tt);
        auto new_end = remove_if(it->second.data.begin(), it->second.data.begin() + (int)it->second.data.size(), pred);
        it->second.data.resize((size_t)(new_end - it->second.data.begin()));
      }
      else if(opea == ">"){
        delete_functor_greater pred(col,tt);
        auto new_end = remove_if(it->second.data.begin(), it->second.data.begin() + (int)it->second.data.size(), pred);
        it->second.data.resize((size_t)(new_end - it->second.data.begin()));
      }
      else{
        cout << "Error: unrecognized operator!\n";
        assert(false);
      }

      if(it->second.index.first == "hash"){
        string index = "hash";
        size_t col_index = it->second.index.second;
        generate_helper(index,col_index,it);

      }
      if(it->second.index.first == "bst"){
        string index = "bst";
        size_t col_index = it->second.index.second;
        generate_helper(index,col_index,it);
      }

      cout << "Deleted " << old_size - it->second.data.size() << " rows from " << name << "\n";
    }

    else if(t == EntryType::Bool){
      size_t old_size = it->second.data.size();
      bool value;
      cin >> value;
      TableEntry tt{value};
      if(opea == "="){
        delete_functor_equal pred(col,tt);
        auto new_end = remove_if(it->second.data.begin(), it->second.data.begin() + (int)it->second.data.size(), pred);
        it->second.data.resize((size_t)(new_end - it->second.data.begin()));
      }
      else if(opea == "<"){
        delete_functor_less pred(col,tt);
        auto new_end = remove_if(it->second.data.begin(), it->second.data.begin() + (int)it->second.data.size(), pred);
        it->second.data.resize((size_t)(new_end - it->second.data.begin()));
      }
      else if(opea == ">"){
        delete_functor_greater pred(col,tt);
        auto new_end = remove_if(it->second.data.begin(), it->second.data.begin() + (int)it->second.data.size(), pred);
        it->second.data.resize((size_t)(new_end - it->second.data.begin()));
      }
      else{
        cout << "Error: unrecognized operator!\n";
        assert(false);
      }

      if(it->second.index.first == "hash"){
        string index = "hash";
        size_t col_index = it->second.index.second;
        generate_helper(index,col_index,it);

      }
      if(it->second.index.first == "bst"){
        string index = "bst";
        size_t col_index = it->second.index.second;
        generate_helper(index,col_index,it);
      }

      cout << "Deleted " << old_size - it->second.data.size() << " rows from " << name << "\n";
    }

    else{
      // unreachable
      cout << "Error: Unregcognized type\n";
    }
  }
  else{
    cout << "Error during DELETE: " << name << " does not name a table in the database\n";
    string trash;
    getline(cin,trash);
    return;
  }
}

void silly::join(){
  string name_1, trash, name_2, col_name_1, col_name_2;
  size_t num_col_print;
  cin >> name_1 >> trash >> name_2 >> trash >> col_name_1 >> trash >> col_name_2 >> trash >> trash >> num_col_print;
  auto it_1 = dataset.find(name_1);
  auto it_2 = dataset.find(name_2);
  if(it_1 != dataset.end() && it_2 != dataset.end()){
    // first find the col index of matching names 
    size_t col_name_1_index, col_name_2_index;
    auto it_3 = it_1->second.header.find(col_name_1);
    if(it_3 != it_1->second.header.end()){
      col_name_1_index = it_3->second;
    }
    else{
      cout << "Error during JOIN: " << col_name_1 << " does not name a column in " << name_1 << "\n";
      string rubbish;
      getline(cin,rubbish);
      return;
    }

    auto it_4 = it_2->second.header.find(col_name_2);
    if(it_4 != it_2->second.header.end()){
      col_name_2_index = it_4->second;
    }
    else{
      cout << "Error during JOIN: " << col_name_2 << " does not name a column in " << name_2 << "\n";
      string rubbish;
      getline(cin,rubbish);
      return;
    }

    /*bool found_1 = false;
    bool found_2 = false; 
    // for colname 1
    for(size_t a = 0; a < it_1->second.col_names.size(); ++a){
      if(it_1->second.col_names[a] == col_name_1){
        col_name_1_index = a;
        found_1 = true;
        break;
      }
    }
    if(!found_1){
      cout << "Error during JOIN: " << col_name_1 << " does not name a column in " << name_1 << "\n";
      string rubbish;
      getline(cin,rubbish);
      return;
    }
    // for colname 2
    for(size_t b = 0; b < it_2->second.col_names.size(); ++b){
      if(it_2->second.col_names[b] == col_name_2){
        col_name_2_index = b;
        found_2 = true;
        break;
      }
    }
    if(!found_2){
      cout << "Error during JOIN: " << col_name_2 << " does not name a column in " << name_2 << "\n";
      string rubbish;
      getline(cin,rubbish);
      return;
    }*/

    // find the col index of printed colmns
    // first is the index in col_names, second is 1 or 2
    vector<pair<size_t,size_t> > col_index;
    vector<string> col_names;
    col_index.reserve(num_col_print);
    col_names.reserve(num_col_print);
    string col_name;
    size_t one_or_two;
    for(size_t i = 0; i < num_col_print; ++i){
      cin >> col_name >> one_or_two;
      if(one_or_two == 1){
        /*bool flag = false;
        for(size_t j = 0; j < it_1->second.col_names.size();++j){
          if(it_1->second.col_names[j] == col_name){
            col_index.emplace_back(j,1);
            flag = true;
            break;
          }
        }
        if(!flag){
          cout << "Error during JOIN: " << col_name << " does not name a column in " << name_1 << "\n";
          string rubbish;
          getline(cin,rubbish);
          return;
        }*/
        auto it_5 = it_1->second.header.find(col_name);
        if(it_5 != it_1->second.header.end()){
          col_index.emplace_back(it_5->second,1);
          col_names.push_back(col_name);
        }
        else{
          cout << "Error during JOIN: " << col_name << " does not name a column in " << name_1 << "\n";
          string rubbish;
          getline(cin,rubbish);
          return;
        }
      }

      else if(one_or_two == 2){
        /*bool flag = false;
        for(size_t j = 0; j < it_2->second.col_names.size();++j){
          if(it_2->second.col_names[j] == col_name){
            col_index.emplace_back(j,2);
            flag = true;
            break;
          }
        }
        if(!flag){
          cout << "Error during JOIN: " << col_name << " does not name a column in " << name_2 << "\n";
          string rubbish;
          getline(cin,rubbish);
          return;
        }*/
        auto it_6 = it_2->second.header.find(col_name);
        if(it_6 != it_2->second.header.end()){
          col_index.emplace_back(it_6->second,2);
          col_names.push_back(col_name);
        }
        else{
          cout << "Error during JOIN: " << col_name << " does not name a column in " << name_2 << "\n";
          string rubbish;
          getline(cin,rubbish);
          return;
        }

      }
      else{
        cout <<"Error: can only join two tables!\n";
        string rubbish;
        getline(cin,rubbish);
        return;
      }
    } // End of the for loop for read in column index

    // Printed the colnames summary
    if(!options.quiet_mode){
      for(size_t n = 0; n < col_names.size(); ++n){
        /*if(col_index[n].second == 1){
          cout << it_1->second.col_names[col_index[n].first] << " ";
        }
        else{
          cout << it_2->second.col_names[col_index[n].first] << " ";
        }*/
        cout << col_names[n] << " ";
      }
      cout << "\n";
    }
    
    // Find matching rows and print
    size_t count = 0;
    if(it_2->second.index.first == "hash" && it_2->second.index.second == col_name_2_index){
      for(size_t k = 0; k < it_1->second.data.size(); ++k){
        auto it_temp = it_2->second.hash.find(it_1->second.data[k][col_name_1_index]);
        if(it_temp != it_2->second.hash.end()){
          if(!options.quiet_mode){
            for(size_t z = 0; z < it_temp->second.size(); ++z){
               for(size_t m = 0; m < col_index.size(); ++m){
                if(col_index[m].second == 1){
                  cout << it_1->second.data[k][col_index[m].first] << " ";
                }
                else{
                  cout << it_2->second.data[it_temp->second[z]][col_index[m].first] << " ";
                }
              }
              cout << "\n";
              ++count;
            }
          }
          else{
            count += it_temp->second.size();
          }
        }
      } 
    } // End of hash join

    else if(it_2->second.index.first == "bst" && it_2->second.index.second == col_name_2_index){
      for(size_t k = 0; k < it_1->second.data.size(); ++k){
        auto it_temp = it_2->second.bst.find(it_1->second.data[k][col_name_1_index]);
        if(it_temp != it_2->second.bst.end()){
          if(!options.quiet_mode){
            for(size_t z = 0; z < it_temp->second.size(); ++z){
               for(size_t m = 0; m < col_index.size(); ++m){
                if(col_index[m].second == 1){
                  cout << it_1->second.data[k][col_index[m].first] << " ";
                }
                else{
                  cout << it_2->second.data[it_temp->second[z]][col_index[m].first] << " ";
                }
              }
              cout << "\n";
              ++count;
            }
          }
          else{
            count += it_temp->second.size();
          }
        }
      } 
    } // End of bst join

    else{
      unordered_map<TableEntry,vector<size_t> > temp;
      for(size_t i = 0; i < it_2->second.data.size(); ++i){
        auto itt = temp.find(it_2->second.data[i][col_name_2_index]);
        if(itt == temp.end()){
          vector<size_t> vec;
          vec.push_back(i);
          temp.insert(make_pair(it_2->second.data[i][col_name_2_index],vec));
        }
        else{
          itt->second.push_back(i);
        }
      }

      for(size_t k = 0; k < it_1->second.data.size(); ++k){
        auto it_temp = temp.find(it_1->second.data[k][col_name_1_index]);
        if(it_temp != temp.end()){
          if(!options.quiet_mode){
            for(size_t z = 0; z < it_temp->second.size(); ++z){
               for(size_t m = 0; m < col_index.size(); ++m){
                if(col_index[m].second == 1){
                  cout << it_1->second.data[k][col_index[m].first] << " ";
                }
                else{
                  cout << it_2->second.data[it_temp->second[z]][col_index[m].first] << " ";
                }
              }
              cout << "\n";
              ++count;
            }
          }
          else{
            count += it_temp->second.size();
          }
        }
      } 
      /*for(size_t k = 0; k < it_1->second.data.size(); ++k){
        for(size_t l = 0; l < it_2->second.data.size(); ++l){
          if(it_1->second.data[k][col_name_1_index] == it_2->second.data[l][col_name_2_index]){
            if(!options.quiet_mode){
              for(size_t m = 0; m < col_index.size(); ++m){
                if(col_index[m].second == 1){
                  cout << it_1->second.data[k][col_index[m].first] << " ";
                else{
                  cout << it_2->second.data[l][col_index[m].first] << " ";
                }
              }
              cout << "\n";
            }
          ++count;
          }
        }
      }*/
    } // End or normal join
    cout << "Printed " << count << " rows from joining " << name_1 << " to " << name_2 << "\n";
  } // End of no index join
  else{
    if(it_1 == dataset.end()){
      cout << "Error during JOIN: " << name_1 << " does not name a table in the database\n";
      string rubbish;
      getline(cin,rubbish);
      return;
    }
    else{
      cout << "Error during JOIN: " << name_2 << " does not name a table in the database\n";
      string rubbish;
      getline(cin,rubbish);
      return;
    }
  }
}// End od Join

void silly::generate(){
  string trash, table_name, index, col_name;
  cin >> trash >> table_name >> index >> trash >> trash >> col_name;
  auto it = dataset.find(table_name);
  // Error checking tablename
  if(it == dataset.end()){
    cout << "Error during GENERATE: " << table_name << " does not name a table in the database\n";
    string rubbish;
    getline(cin,rubbish);
    return;
  }
  size_t col_index = 0;
  auto it_7 = it->second.header.find(col_name);
  if(it_7 != it->second.header.end()){
    col_index = it_7->second;
  }
  else{
    cout << "Error during GENERATE: " << col_name << " does not name a column in " << table_name << "\n";
    string rubbish;
    getline(cin,rubbish);
    return;
  }
  /*bool found = false;
  //find colname index
  for(size_t i = 0; i < it->second.col_names.size(); ++i){
    if(it->second.col_names[i] == col_name){
      col_index = i;
      found = true;
      break;
    }
  }
  // Error checking col name
  if(!found){
    cout << "Error during GENERATE: " << col_name << " does not name a column in " << table_name << "\n";
    string rubbish;
    getline(cin,rubbish);
    return;
  }*/

  // Error checking index type
  if(index != "hash" && index != "bst"){
    cout << "Error: invalid index type!\n";
    string rubbish;
    getline(cin,rubbish);
    return;
  }
  
  // if exactly the same index already existed
  if(index == it->second.index.first && col_index == it->second.index.second){
    cout << "Created " << index << " index for table " << table_name << " on column " << col_name << "\n";
    return;
  }
  
  // if all valid, call the helper
  generate_helper(index,col_index,it);
  cout << "Created " << index << " index for table " << table_name << " on column " << col_name << "\n";
}

template<typename Iterator>
void silly::generate_helper(string &index, size_t col_index, Iterator &it) const{
  it->second.hash.clear();
  it->second.bst.clear();

  if(index == "hash"){
    it->second.index = make_pair("hash",col_index);
    for(size_t j = 0; j < it->second.data.size(); ++j){
      auto it_h = it->second.hash.find(it->second.data[j][col_index]);
      if(it_h == it->second.hash.end()){
        vector<size_t> temp;
        temp.push_back(j);
        it->second.hash.insert(make_pair(it->second.data[j][col_index],temp));
      }
      else{
        it_h->second.push_back(j);
      }
    }
  }
  else{
    it->second.index = make_pair("bst",col_index);
    for(size_t j = 0; j < it->second.data.size(); ++j){
      auto it_b = it->second.bst.find(it->second.data[j][col_index]);
      if(it_b == it->second.bst.end()){
        vector<size_t> temp;
        temp.push_back(j);
        it->second.bst.insert(make_pair(it->second.data[j][col_index],temp));
      }
      else{
        it_b->second.push_back(j);
      }
    }
  }
}// End of generator_helper

int main(int argc, char ** argv){
    ios_base::sync_with_stdio(false);
    xcode_redirect(argc,argv);
    cin >> std::boolalpha; 
    cout << std::boolalpha;
    silly s;
    s.get_options(argc,argv);
    s.read();
    return 0;
}
