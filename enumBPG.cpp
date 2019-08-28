#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

using namespace std;

enum Parenthesis{LEFT, RIGHT};
stack<int> recover_point;
vector<int> connected_value_top;
vector<int> connected_value_bottom;

unsigned long numBPG = 0;
bool isParent = true;
bool update_is_s1 = true;
bool update_connected_value = false;
bool updateS1 = false;

class Sequence{
public:
  int p, q;
  vector<int> s1;
  vector<int> s2;

  Sequence(int p, int q){
    this->p = p; this->q = q;
    for(int i=0 ; i<p ; i++) s1.push_back(LEFT);
    for(int i=0 ; i<q ; i++) s1.push_back(RIGHT);
    for(int i=0 ; i<q ; i++) s2.push_back(LEFT);
    for(int i=0 ; i<p ; i++) s2.push_back(RIGHT);

    if(s1.size() != s2.size()) exit(-1);
  }

  Sequence(vector<int> s1, vector<int> s2){
    this->s1 = s1;
    this->s2 = s2;
  }

  Sequence flipVertical(){
    vector<int> sv1, sv2;
    for(int i=s1.size()-1 ; i>=0 ; i--){
      if(s1[i] == LEFT) sv1.push_back(RIGHT);
      else sv1.push_back(LEFT);
    }
    for(int i=s2.size()-1 ; i>=0 ; i--){
      if(s2[i] == LEFT) sv2.push_back(RIGHT);
      else sv2.push_back(LEFT);
    }
    return Sequence(sv1, sv2);
  }

  Sequence flipHorizontal(){
    return Sequence(s2, s1);
  }

  Sequence rotate(){
    Sequence sv = flipVertical();
    return sv.flipHorizontal();
  }

  bool isCanonical(){
    Sequence sr = rotate();
    if(sr < *this) return false;

    if(p == q){
      Sequence sv = flipVertical();
      if(sv < *this) return false;
      
      Sequence sh = flipHorizontal();
      if(sh < *this) return false;
    }
    return true;
  }

  bool isConnected(){
    int c1, c2;
    c1 = c2 = 0;
    int update_value_0, update_value_1 = 0;
    if(isParent == 1){
      for(int i=0 ; i<s1.size()-1 ; i++){
        if(s1[i] == LEFT) c1++;
        else c1--;
        if(s2[i] == RIGHT) c2++;
        else c2--;
        if(c1 == c2) return false;

        connected_value_top.push_back(c1);
        connected_value_bottom.push_back(c2);
      }
    }
     else if(update_is_s1 == true) {
      update_value_0 = connected_value_top[recover_point.top()] - 2;
      update_value_1 = update_value_0 + 1;
      if(update_value_0 == connected_value_bottom[recover_point.top()]) return false;
      if(update_value_1 == connected_value_bottom[recover_point.top()+1]) return false;

      connected_value_top[recover_point.top()] = update_value_0;
      connected_value_top[recover_point.top()+1] = update_value_1;
    }
    else{
      update_value_0 = connected_value_bottom[recover_point.top()] + 2;
      update_value_1 = update_value_0 - 1;
      if(connected_value_top[recover_point.top()] == update_value_0) return false;
      if(connected_value_top[recover_point.top()+1] == update_value_1) return false;

      connected_value_bottom[recover_point.top()] = update_value_0;
      connected_value_bottom[recover_point.top()+1] = update_value_1;
    }
    return true;
  }

  bool operator<(const Sequence& rhs){
    for(int i=0 ; i<s1.size() ; i++){
      if(s1[i] == rhs.s1[i]) continue;
      if(s1[i] == LEFT) return true;
      else return false;
    }
    for(int i=s2.size()-1 ; i>=0 ; i--){
      if(s2[i] == rhs.s2[i]) continue;
      if(s2[i] == RIGHT) return true;
      else return false;
    }
    return false;
  }

  void output(){
    for(int i=0 ; i<s1.size() ; i++){
      if(s1[i]==LEFT) cout << "0";
      else cout << "1";
    }
    cout << " ";
    for(int i=0 ; i<s2.size() ; i++){
      if(s2[i]==LEFT) cout << "0";
      else cout << "1";
    }
    cout << endl;
  }

  bool equalS2(vector<int> s2){ return this->s2 == s2;}
  bool equal(vector<int> s1, vector<int> s2){
    return (this->s1 == s1 && this->s2 == s2);
  }
  bool operator==(const Sequence& rhs){
    return (this->s1 == rhs.s1 && this->s2 == rhs.s2);
  }

};

void recover_connected_value_s1(){
  int update_value = 0;
  connected_value_top[recover_point.top()] += 2;
  update_value = connected_value_top[recover_point.top()];
  connected_value_top[recover_point.top()+1] = update_value - 1;
}

void recover_connected_value_s2(){
  int update_value = 0;
  connected_value_bottom[recover_point.top()] -= 2;
  update_value = connected_value_bottom[recover_point.top()];
  connected_value_bottom[recover_point.top()+1] = update_value + 1;
}


void enumeration(Sequence& s){
  if(!s.isCanonical() || !s.isConnected()){
    // if(!s.isCanonical()) printf("not canonical \n");
    // if(!s.isConnected()) printf("not connected \n");
    return;
  }

  s.output();
  numBPG++;
  isParent = 0;

  if(updateS1 == false){
    // case 1
    for(int i=s.s2.size()-1 ; i>0 ; i--){
      if(s.s2[i-1] == LEFT && s.s2[i] == RIGHT){
        swap(s.s2[i], s.s2[i-1]);
        recover_point.push(i-1);
        update_is_s1 = false;
        enumeration(s);
        if(update_connected_value == true) recover_connected_value_s2();
        update_connected_value = false;
        swap(s.s2[recover_point.top()], s.s2[recover_point.top()+1]);
        recover_point.pop();
        break;
      }
    }
    // case 2
    bool flag = false; // maintain Rightmost "]["
    for(int i=s.s2.size()-1 ; i>0 ; i--){
      if(flag && s.s2[i] == LEFT) continue;
      
      if(flag && s.s2[i] == RIGHT){
        if(s.s2[i-1] == LEFT){
          swap(s.s2[i], s.s2[i-1]);
          recover_point.push(i-1);
          update_is_s1 = false;
          enumeration(s);
          if(update_connected_value == true) recover_connected_value_s2();
          update_connected_value = false;
          swap(s.s2[recover_point.top()], s.s2[recover_point.top()+1]);
          recover_point.pop();
        }
        break;
      }
      if(!flag && s.s2[i-1] == LEFT && s.s2[i] == RIGHT) flag = true;
    }
    // case 3 (swap on s1)
    for(int i=0 ; i<s.s1.size() ; i++){
      if(s.s1[i] == LEFT && s.s1[i+1] == RIGHT){
        swap(s.s1[i], s.s1[i+1]);
        recover_point.push(i);
        update_is_s1 = true;
        updateS1 = true;
        enumeration(s);
        updateS1 = false;
        if(update_connected_value == true) recover_connected_value_s1();
        update_connected_value = false;
        swap(s.s1[recover_point.top()], s.s1[recover_point.top()+1]);
        recover_point.pop();
        break;
      }
    }
  }
  else{
    // case 1
    for(int i=0; i<s.s1.size() ; i++){
      if(s.s1[i] == LEFT && s.s1[i+1] == RIGHT){
        swap(s.s1[i], s.s1[i+1]);
        recover_point.push(i);
        update_is_s1 = true;
        updateS1 = true;
        enumeration(s);
        updateS1 = false;
        if(update_connected_value == true) recover_connected_value_s1();
        update_connected_value = false;
        swap(s.s1[recover_point.top()], s.s1[recover_point.top()+1]);
        recover_point.pop();
        break;
      }
    }
    // case 2
    bool flag = false; // leftmost "]["
    for(int i=0 ; i<s.s1.size() ; i++){
      if(flag && s.s1[i] == RIGHT) continue;
      
      if(flag && s.s1[i] == LEFT){
        if(s.s1[i+1] == RIGHT){
          swap(s.s1[i], s.s1[i+1]);
          recover_point.push(i);
          update_is_s1 = true;
          updateS1 = true;
          enumeration(s);
          updateS1 = false;
          if(update_connected_value == true) recover_connected_value_s1();
          update_connected_value = false;
          swap(s.s1[recover_point.top()], s.s1[recover_point.top()+1]);
          recover_point.pop();
        }
        break;
      }
      if(s.s1[i] == LEFT && s.s1[i+1] == RIGHT) flag = true;
    }
  }
  update_connected_value = true;
}


void enumeration(int p, int q){
  Sequence s(p, q);
  isParent = true;
  update_is_s1 = true;
  enumeration(s);
}


int main(int argc, char** argv){
  int n;
  cout << "Please input a number of vertices: ";
  cin >> n;

  for(int j=n ; j<=n ; j++){
    numBPG = 0;
    for(int i=1 ; i<=j/2 ; i++){
      cerr << "p, q: " << j-i << ", " << i << endl;
      connected_value_top.clear();
      connected_value_bottom.clear();
      enumeration(j-i, i);
      cout << numBPG << endl;
    }
  }
}
