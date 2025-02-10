#include<iostream>
using namespace std;
void update(string box[]);

string box[9]={"1","2","3",
               "4","5","6",
              "7","8","9"};

string player="*";
int choice;
int main(){
    update(box);
    while (true)
    {
     cout<<"player " <<player<<":";
     cin>>choice;
     if (choice<9 && choice>0 && box[choice]!="*" && box[choice]!="#"){
        box[choice]=player;
        update(box);
        if (player=="*"){
            player="#";
        }else{
            player="*";
        }
     }
    }
    
}
void update(string box[]){
    for(int x=0;x<3;x++){
        for(int i=0;i<3;i++){
            cout<<"|"<<box[(i+x*3)];
        }
        cout<<"|"<<endl;
    }
}
