#include<iostream>
using namespace std;
void update(string box[]);
string chek(string box[]);
string player="*";
int choice;
int main(){
    string box[9]={"1","2","3",
                   "4","5","6",
                   "7","8","9"};
    update(box);
    while (true)
    {
     if(chek(box) =="*" || chek(box)=="#" || chek(box)=="over" ) {
        break;
     }
     cout<<"player " <<player<<":";
     cin>>choice;
     choice--;
     if (choice<9 && choice>=0 && box[choice]!="*" && box[choice]!="#"){
        box[choice]=player;
        if (player=="*"){
            player="#";
        }else{
            player="*";
        }
        update(box);
     }
    }
    while(true){
        string a;
        cout<<"press a to try again"<<endl;
        cin>>a;
        if(a=="a"){
            main();
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

string  chek(string box[]){
    string msg="over";
    for(int x =0;x<3;x++){
        if(box[3*x]==box[1+3*x] && box[1+3*x]==box[2+3*x] && (box[3*x] =="*" || box[3*x]=="#")){
            return box[3*x];
        }
        if(box[x]==box[x+3] && box[x+3]==box[x+6] && (box[x] =="*" || box[x]=="#")){
            return box[x];
        }
    }
    if(box[0]==box[4] && box[4]==box[8] && (box[8] =="*" || box[8]=="#")){
        return box[0];
    }
    if(box[2]==box[4] && box[4]==box[6] && (box[6] =="*" || box[6]=="#")){
        return box[2];
    }
    for(int x=1;x<=9;x++){
        if (to_string(x)==box[x-1]){
            msg="no";
        }
    }
    return msg;
}