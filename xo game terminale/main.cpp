#include<iostream>
using namespace std;
void update(string box[]);

string box[9]={"1","2","3",
               "4","5","6",
               "7","8","9"};
int main(){
    update(box);
}
void update(string box[]){
    int a;
    for(int x=0;x<3;x++){
        for(int i=0;i<3;i++){
            cout<<"|"<<box[(i+x*3)];
        }
        cout<<"|"<<endl;
    }
    cin>>a;
}
