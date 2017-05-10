#include <iostream>
#include <stdio.h>
#include <string>
#include<vector>
using namespace std;


const int SIZE = 256;//the size of the alphabet (here, we used the size of ASCII)
const int MAXN = 5000;//the max length of the string


int pos;//the pointer of the char which is being insert

int root; // the root of tree
int last_added; // the number of the nodes that have been created, thus it records the number of the node, created last.
int needSL;     //  it describes if it has created a new node in this period. if so, it need to record the number of the new node and update the suffix list.
int remainder;   //  it describes how many characters need to be insert in this period.
int active_node, active_e, active_length;   // the three elements of active point

struct node {

    //  every node records a group of continuous characters
    // [start,end) in the primary string
    // for example: start=3,end=5. the string used to create suffix tree is 'abpnhgscn' , so  the node represents 'pn' in this case

    int start,end, link;  //link : initialized as 0. as we know, suffix list records the next node created in the same period, and link records that
    int sonnum;   //  the number of sons.
    vector <int>son;  // record the number of every son

    int next[SIZE];   //  record if the char exits
    /*eg:

        a(node number is 1)-------bc(node number is 2)------...
        |
        |
         -------dea(node number is 3)---.....

         then in the node(1) of 'a' , next['b']=2 ; next['d']=3

    */
    int edge_length() {   //return the length of the edge
        return min(end, pos + 1) - start;
    }
};
class suffix_tree{
public:

    suffix_tree(){
        st_init();
    };


    node tree[2*MAXN];   //  record all node of the tree
    char text[MAXN];    //  record the string used to create the suffix tree

    int new_node(int start, int end = MAXN) {  //  create a new node
        node nd;
        nd.start = start;
        nd.end = end;
        nd.link = 0;    // link is initialized as 0
        nd.sonnum=0;
        for (int i = 0; i < SIZE; i++)
            nd.next[i] = 0;         //  initialization
        tree[++last_added] = nd;    //  record the new node in tree array
        return last_added;
    }

    char active_edge() {    //  return the real char of active_e
        return text[active_e];
    }

    void add_SL(int node) {    //  update the suffix list
        if (needSL > 0) tree[needSL].link = node;  //  this is not the first node created in this period
        needSL = node;   // this is the first node in this period
    }

    bool walk_down(int node) {  // go to a new node and update the active point
        if (active_length >= tree[node].edge_length()) {
            active_e += tree[node].edge_length();
            active_length -= tree[node].edge_length();
            active_node = node;
            return true;
        }
        return false;
    }

    void st_init() {  //initialization
        needSL = 0, last_added = 0, pos = -1,
        remainder = 0, active_node = 0, active_e = 0, active_length = 0;
        root = active_node = new_node(-1, -1);
    }

    void st_extend(char c) {  // insert char(c) into the tree
        text[++pos] = c;  // record this char in text array
        needSL = 0;     //  update the needSL  every period
        remainder++;    // add the reminder every period
        while(remainder > 0) {
            if (active_length == 0) active_e = pos;

            if (tree[active_node].next[active_edge()] == 0) {

                int leaf = new_node(pos);    // not find the char in active point , so add a leaf node
                tree[active_node].next[active_edge()] = leaf;
                add_SL(active_node); // according to rule 2 , update the suffix list

            } else {

                int nxt = tree[active_node].next[active_edge()];
                if (walk_down(nxt))
                    continue;      //  if it gets a new node, update the active point
                if (text[tree[nxt].start + active_length] == c) { //  if  find the char in active point, we just update the active length
                    active_length++;
                    add_SL(active_node);
                    break;
                }

                // if we don't find the char c in active point, we split the point and add a leaf node

                int split = new_node(tree[nxt].start, tree[nxt].start + active_length);
                tree[active_node].next[active_edge()] = split;

                int leaf = new_node(pos);
                tree[split].next[c] = leaf;
                tree[nxt].start += active_length;
                tree[split].next[text[tree[nxt].start]] = nxt;
                add_SL(split);

            }
            remainder--;
            if (active_node == root && active_length > 0) { //according to rule 1,the insertion  happens in root

                active_length--;
                active_e = pos - remainder + 1;

            } else  // according to rule 3, the insertion does not happen in root
                active_node = tree[active_node].link > 0 ? tree[active_node].link : root;
        }
    }

// when the tree is finished, this function finds all son of every node and aggregate the sum in order to access the son node conveniently.
    void dfs(int x){
        tree[x].sonnum=0;
        for(int i=1;i<SIZE;i++){
            if(tree[x].next[i]!=0)
            {
                tree[x].son.push_back(tree[x].next[i]);
                tree[x].sonnum++;
                dfs(tree[x].next[i]);
            }
        }
    }


    bool find_t_from_s(string t,string s){


        st_init();
        for(int i=0;i<s.length();i++){
            st_extend(s[i]);
        }
        st_extend('$');

    //    dfs(1);
        int p_t=0;
        int p_s=1;
        while(true){
            if(tree[p_s].next[t[p_t]]==0){
                return false;
            }
            p_s=tree[p_s].next[t[p_t]];
            int start=tree[p_s].start;
            int tail=tree[p_s].end;
            for(int i=start;i<tail;i++){
                if(s[i]!=t[p_t]){
                    return false;
                }
                p_t++;
                if(p_t==t.length()){
                    return true;
                }
            }
        }
    }


// dfs_count_leaf find how many leaves the node has and return the num
    void dfs_count_leaf(int *num,int node){

        if(tree[node].sonnum==0){
            (*num)++;
            return;
        }
        for(int i=0;i<tree[node].sonnum;i++){
            dfs_count_leaf(num,tree[node].son[i]);
        }
        return;
    }

//  count_t_in_s is a function that count how many t occurs in string s

    int count_t_in_s(string t,string s){

        st_init();
        for(int i=0;i<s.length();i++){
            st_extend(s[i]);
        }
        st_extend('$');

        dfs(1);
        int p_t=0;
        int p_s=1;
        while(true){
            if(tree[p_s].next[t[p_t]]==0){
                return 0;
            }
            p_s=tree[p_s].next[t[p_t]];
            int start=tree[p_s].start;
            int tail=tree[p_s].end;
            for(int i=start;i<tail;i++){
                if(s[i]!=t[p_t]){
                    return 0;
                }
                p_t++;
                if(p_t==t.length()){
                    goto breakloop;
                }
            }
        }
        breakloop:
        int num=0;
        dfs_count_leaf(&num,p_s);
        return num;
    }


    int dfs_deep(int node,int depth,int *head,int *tail){

        if(tree[node].sonnum==0)
            return depth;
        int m=0;
        int h=0;
        int t=0;
        for(int i=0;i<tree[node].sonnum;i++){
            *tail=tree[node].end;
            int result=dfs_deep(tree[node].son[i],depth+tree[node].edge_length(),head,tail);
            if(result>m){
                m=result;
                h=*head;
                t=*tail;
            }
        }
        *head=h;
        *tail=t;
        return m;

    }

// find the longest substring in  string s.
    string find_longest_in_s(string s){

        st_init();
        for(int i=0;i<s.length();i++){
            st_extend(s[i]);
        }
        st_extend('$');
        dfs(1);

        int head=0;
        int tail=0;

        int m=0,t=0,h=0;
        for(int i=0;i<tree[1].sonnum;i++){
            head=tree[tree[1].son[i]].start;
            tail=tree[tree[1].son[i]].end;
            int result=dfs_deep(tree[1].son[i],0,&head,&tail);
            if(result>m){
                t=tail;
                h=head;
                m=result;
            }
        }
        return s.substr(h,t-h);

    }

};
int main() {







//    st_init();
//    char a[]="abcabxabcd";
//    for(int i=0;i<11;i++){
//        st_extend(a[i]);
//    }
//    for(int i=0;i<256;i++){
//        if(tree[1].next[i]!=0){
//            cout<<"tree[0].next[i]  :"<<tree[1].next[i]<<"   start:"<<tree[tree[1].next[i]].start<<"  end:"<<tree[tree[1].next[i]].end<<endl;
//        }
//    }
//    cout<<endl;
//    dfs(1);


//    string s="banana";
//    string t=" ";
//    cout<<find_t_from_s(t,s);

//
//    string s="banana";
//    string t="a";
//    cout<<a->count_t_in_s(t,s);


//    string s="banabanab";
//    cout<<find_longest_in_s(s);

//    string s="babbcb";
//    string t="b";
//    cout<<find_longest_common(t,s);

    suffix_tree *a = new suffix_tree();

    while(true){
        int n=0;
        string s;
        string t;
        cout<<"请输入要解决的问题编号（1-3）："<<endl;
        cout<<"1、查找一个字符串S是否包含子串T。"<<endl;
        cout<<"2、统计S中T出现的次数。"<<endl;
        cout<<"3、找出S中最长的重复子串。"<<endl;

        cin>>n;

        switch(n){

            case 1:
            {

                cout<<"请输入字符串S"<<endl;
                cin>>s;
                cout<<"请输入字符串T"<<endl;
                cin>>t;
                bool ans=a->find_t_from_s(t,s);
                if(ans)
                    cout<<"S中包含T"<<endl;
                else
                    cout<<"S中不包含T"<<endl;
                break;
            }

            case 2:
            {

                cout<<"请输入字符串S"<<endl;
                cin>>s;
                cout<<"请输入字符串T"<<endl;
                cin>>t;
                int ans = a->count_t_in_s(t,s);
                cout<<"S中T出现了"<<ans<<"次。"<<endl;
                break;
            }

            case 3:
            {

                cout<<"请输入字符串S"<<endl;
                cin>>s;
                string t=a->find_longest_in_s(s);
                 if(t!="")
                    cout<<"S中最长的重复子串是"<<t<<endl;
                else
                    cout<<"S中不存在重复子串"<<endl;
                break;
            }


            default:
                cout<<"请重新输入"<<endl;
         }
    }
    return 0;
}
