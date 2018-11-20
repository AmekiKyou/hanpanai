//Greedy Hanpanaitte

#include<iostream>
#include<utility>
#include<vector>
#include<cmath>
#include<random>
#include<chrono>
#include<map>

bool inrange(int x,int y){
  return 0<=x&&x<5&&0<=y&&y<5;
}

const std::string DIR="LDRU";
const int _dir[4][2]={{-1,0},{0,-1},{1,0},{0,1}};

namespace PlayBoard{

class PlayBoard{
 private:
   int Board[5][5];
 public:
   PlayBoard(){
     for (int i=0;i<5;++i)
      for (int j=0;j<5;++j) Board[i][j]=0;
   }

   void update(int updated[5][5]){
     for (int i=0;i<5;++i)
      for (int j=0;j<5;++j) Board[i][j]=updated[i][j];
   }

   struct moveLret{
     char dir;
     int movenum;
     int movescore;
     int safety;
     int board[5][5];
     moveLret(char d, int num, int score, int bd[5][5]):dir(d),movenum(num),movescore(score){
       for (int i=0;i<5;++i)
        for (int j=0;j<5;++j)
          board[i][j]=bd[i][j];
       safety=0;
       int safer[5]={-3,-3,-3,-3,-3},safec[5]={-3,-3,-3,-3,-3};
       for (int i=0;i<5;++i)
        for (int j=0;j<5;++j)
          if (board[i][j]==0){
            ++safety;
            safer[i]=0;
            safec[j]=0;
          }
          else{
            int signal=-2;
            for (int d=0;d<4;++d){
              int px=i+_dir[d][0],py=j+_dir[d][1];
              if (!inrange(px,py)) continue;
              if (board[px][py]==board[i][j]){
                signal=1;
                safer[i]=0;
                safec[j]=0;
                break;
              }
            }
            if (signal==-2){
              for (int d=0;d<4;++d){
                int px=i+_dir[d][0],py=j+_dir[d][1];
                if (!inrange(px,py)) continue;
                if (!board[px][py]){
                  ++signal;
                  if (signal==0) break;
                }
              }
            }
            safety+=signal;
          }
          for (int i=0;i<5;++i) safety+=safer[i];
          for (int j=0;j<5;++j) safety+=safec[j];
     }
     void dump(){
       std::cout<<"DIR: "<<dir<<std::endl;
       std::cout<<"NUM: "<<movenum<<std::endl;
       std::cout<<"SCORE: "<<movescore<<std::endl;
       std::cout<<"SAFETY: "<<safety<<std::endl;
       for (int i=0;i<5;++i){
         for (int j=0;j<5;++j) std::cout<<board[i][j]<<" ";
         std::cout<<std::endl;
       }
       std::cout<<std::endl;
     }
   };

   inline moveLret moveL(int d){
     //rotate d times
     //then move left
     int board[5][5];
     for (int i=0;i<5;++i)
      for (int j=0;j<5;++j) board[i][j]=Board[i][j];

     for (int k=0;k<d;++k){
       for (int i=0;i<2;++i)
        for (int j=0;j<3;++j){
          int temp=board[i][j];
          board[i][j]=board[4-j][i];
          board[4-j][i]=board[4-i][4-j];
          board[4-i][4-j]=board[j][4-i];
          board[j][4-i]=temp;
        }
     }

     bool moved=false,standby[5]={true,true,true,true,true};
     int merge=0;
     int pointsgain=0;
     for (int i=0;i<5;++i)
      for (int j=0;j<5;++j)
        if (board[i][j]){
          int k=j;
          while (k>0&&!board[i][k-1]){
            moved=true;
            std::swap(board[i][k-1],board[i][k]);
            --k;
          }
          if (k>0&&standby[i]&&board[i][k-1]==board[i][k]){
            moved=true;
            standby[i]=false;
            board[i][k]=0;
            ++board[i][k-1];
            ++merge;
            pointsgain+=1<<board[i][k-1];
          }
          else standby[i]=true;
        }

     for (int k=d;k<4;++k){
       for (int i=0;i<2;++i)
        for (int j=0;j<3;++j){
          int temp=board[i][j];
          board[i][j]=board[4-j][i];
          board[4-j][i]=board[4-i][4-j];
          board[4-i][4-j]=board[j][4-i];
          board[j][4-i]=temp;
        }
     }

     if (moved) return moveLret(DIR[d], merge, pointsgain, board);
     else return moveLret('\0', -1, -1, board);
   }

   struct moveret{
     char dir;
     int movenum;
     int movescore;
     int secnum;
     int secscore;
     int safety;
     int secsafe;
     moveret(char d, int num, int score):dir(d),movenum(num),movescore(score){}
   };

   moveret move(int level, int rule, bool debug){
     // If you can merge this turn, do not merge in the next turn
     // level: 0 or 1, I am not to predict 2 turns later...
     // rule 0: Merge more, not merge bigger;
     // rule 1: Merge bigger, not merge more;
     // rule 2: rank with 'safety', more rational than rule 0?

     moveret ret('\0',-1,-1);
     ret.safety=-50000;

     for (int i=0;i<4;++i){
       moveLret cur=moveL(i);
       if (cur.dir){
         int secnum=0,secscore=0,secsafe=0;
         if (level==0){
           PlayBoard board_tmp;
           board_tmp.update(cur.board);
           moveret sec_ret=board_tmp.move(level+1, rule, debug);
           secnum=sec_ret.movenum;
           secscore=sec_ret.movescore;
           secsafe=sec_ret.safety;
           if (debug){
             std::cout<<"primary"<<std::endl;
             cur.dump();
             std::cout<<"secondary"<<std::endl;
             std::cout<<"NUM: "<<secnum<<std::endl;
             std::cout<<"SCORE: "<<secscore<<std::endl;
             std::cout<<std::endl;
           }
         }
         if (rule==0){
           if (cur.movenum>ret.movenum||
             (cur.movenum==ret.movenum&&
               (secnum>ret.secnum||
                 (secnum==ret.secnum&&
                   (cur.movescore>ret.movescore||
                     (cur.movescore==ret.movescore&&
                       secscore>ret.secscore)))))){
             ret.movenum=cur.movenum;
             ret.dir=cur.dir;
             ret.movescore=cur.movescore;
             ret.secnum=secnum;
             ret.secscore=secscore;
             ret.safety=cur.safety;
             ret.secsafe=secsafe;
           }
         }
         if (rule==1){
           if (cur.movescore>ret.movescore||
             (cur.movescore==ret.movescore&&
               (secscore>ret.secscore||
                 (secscore==ret.secscore&&
                   (cur.movenum>ret.movenum||
                     (cur.movenum==ret.movenum&&
                       secnum>ret.secnum)))))){
             ret.movenum=cur.movenum;
             ret.dir=cur.dir;
             ret.movescore=cur.movescore;
             ret.secnum=secnum;
             ret.secscore=secscore;
             ret.safety=cur.safety;
             ret.secsafe=secsafe;
           }
         }
         if (rule==2){
           if (cur.safety>ret.safety||
             (cur.safety==ret.safety&&
               (secsafe>ret.secsafe||
                 (secsafe==ret.secsafe&&
                   cur.movescore>ret.movescore)))){
               ret.movenum=cur.movenum;
               ret.dir=cur.dir;
               ret.movescore=cur.movescore;
               ret.secnum=secnum;
               ret.secscore=secscore;
               ret.safety=cur.safety;
               ret.secsafe=secsafe;
             }
         }
       }
     }

     return ret;
   }

   std::vector<std::pair<int,int>> put(int x, int max_retry){
     // somewhat Prim-like strategy + monte-carlo

     unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
     // obtain a seed from the system clock
     std::mt19937 generator(seed);

     std::vector<std::pair<int,int>> ret;
     std::vector<std::pair<int,int>> slot;
     for (int i=0;i<5;++i)
      for (int j=0;j<5;++j)
        if (Board[i][j]==0) slot.push_back(std::make_pair(i,j));
     int winchance=10000,winpoint,winnum;
     // This is for enemy (times to merge), make it smaller
     for (int v=x;v>=1;--v){
       int num_put=1<<(x-v);
       if (num_put>slot.size()) break;
       int num_retry=max_retry;
       while (num_retry--){
         //std::cout<<"retry #"<<max_retry-num_retry<<std::endl;
         int board[5][5];
         for (int i=0;i<5;++i)
          for (int j=0;j<5;++j) board[i][j]=Board[i][j];
         std::vector<std::pair<int,int>> slot_tmp=slot;
         std::vector<std::pair<int,int>> gulag_list;
         while (num_put--){
           int minnxt=10000;
           std::vector<std::pair<int,int>> victims;
           for (std::pair<int,int> candidate:slot_tmp){
             int x=candidate.first,y=candidate.second;
             if (board[x][y]) continue;
             int nxt=0;
             {
               bool same=false;
               for (int i=y-1;i>=0;--i)
                if (board[x][i]){
                  if (board[x][i]==v) same=true;
                  break;
                }
               if (same) ++nxt;
             }
             {
               bool same=false;
               for (int i=y+1;i<5;++i)
                if (board[x][i]){
                  if (board[x][i]==v) same=true;
                  break;
                }
               if (same) ++nxt;
             }
             {
               bool same=false;
               for (int i=x-1;i>=0;--i)
                if (board[i][y]){
                  if (board[i][y]==v) same=true;
                  break;
                }
               if (same) ++nxt;
             }
             {
               bool same=false;
               for (int i=x+1;i<5;++i)
                if (board[i][y]){
                  if (board[i][y]==v) same=true;
                  break;
                }
               if (same) ++nxt;
             }
             //std::cout<<"Next of: "<<x<<" "<<y<<" is "<<nxt<<std::endl;
             if (nxt<minnxt){
               minnxt=nxt;
               victims.clear();
               victims.push_back(candidate);
               //std::cout<<"renewed as victim"<<std::endl;
             }
             else if (nxt==minnxt){
               victims.push_back(candidate);
               //std::cout<<"added to victim"<<std::endl;
             }
           }
           int num_victims=victims.size();
           //std::cout<<"total victim number: "<<num_victims<<std::endl;
           int pos=generator()%num_victims;
           std::pair<int,int> victim=victims[pos];
           board[victim.first][victim.second]=v;
           gulag_list.push_back(victim);
         }
         num_put=1<<(x-v);
         PlayBoard board_tmp;
         board_tmp.update(board);
         moveret moveres_rule0=board_tmp.move(0,0,false);
         moveret moveres_rule1=board_tmp.move(0,1,false);
         moveret moveres_rule2=board_tmp.move(0,2,false);
         if (moveres_rule2.safety<winchance||
           (moveres_rule2.safety==winchance&&
             (moveres_rule1.movescore<winpoint||
               (moveres_rule1.movescore==winpoint&&
                 moveres_rule0.movenum<winnum)))){
                   winchance=moveres_rule2.safety;
                   winpoint=moveres_rule1.movescore;
                   winnum=moveres_rule0.movenum;
                   ret=gulag_list;
                 }
       }
     }
     return ret;
   }
};

};

int main (void){
  int player;
  std::cin>>player;
  if (player) std::cout<<"3 3"<<std::endl;
  else std::cout<<"2 2"<<std::endl;
  // No meaning, just for fun

  int turn, timeleft, score_me, score_rival;
  int board_me[5][5], board_rival[5][5];
  PlayBoard::PlayBoard me, rival;
  std::vector<std::pair<int,int>> putres;
  int time_retry=1500, cd=100;
  int moverule=1;
  while (true){
    std::cin>>turn>>timeleft>>score_me>>score_rival;
    int empty=0;
    for (int i=0;i<5;++i)
      for (int j=0;j<5;++j){
        std::cin>>board_me[i][j];
        if (board_me[i][j]==0) ++empty;
      }
    for (int i=0;i<5;++i)
      for (int j=0;j<5;++j) std::cin>>board_rival[i][j];
    me.update(board_me);
    rival.update(board_rival);
    PlayBoard::PlayBoard::moveret moveres=me.move(0,2,false);
    if (moveres.dir){
      if (empty+moveres.movenum<10) moverule=0;
      else moverule=1;
      if (turn%cd==0&&(100000-timeleft)>100*turn) time_retry/=2;
      if (time_retry<=0) time_retry=1;
      putres=rival.put(moveres.movenum+1, time_retry);
      int m=putres.size();
      int v=moveres.movenum+1-log2(m);
      std::cout<<moveres.dir<<" "<<m<<" "<<v;
      for (auto i: putres) std::cout<<" "<<i.first+1<<" "<<i.second+1;
      std::cout<<std::endl;
    }
    else{
      std::cout<<"Kill me!"<<std::endl;
      break;
    }
  }
  return 0;
}
