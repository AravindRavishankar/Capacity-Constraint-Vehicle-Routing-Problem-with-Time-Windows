#include<bits/stdc++.h>

using namespace std;

typedef pair<int,int> pr;
typedef vector< pr > vec2;
typedef vector<int> vec;
typedef vector< vec > vvec;
typedef long long int lint;

#define PB push_back
#define MP make_pair
#define SZ(a) (int)(a.size())
#define ALL(a) a.begin(),a.end

#define SET(a,b) memset(a,b,sizeof(a))
#define scan(n) scanf("%d",&n)
#define print(n) printf("%d\n",n)
#define scanl(n) scanf("%lld",&n)
#define scans(s) cin>>s
#define printl(n) printf("%lld\n",n)
#define fr(i,s,n) for(int i=s;i<n;i++)
#define mod 1000000007
#define INF 1000000007

//#define TRACE

#ifdef TRACE
#define trace(...) __f(#__VA_ARGS__, __VA_ARGS__)
template <typename Arg1>
void __f(const char* name, Arg1&& arg1){
    cerr << name << " : " << arg1 << std::endl;
}
template <typename Arg1, typename... Args>
void __f(const char* names, Arg1&& arg1, Args&&... args){
    const char* comma = strchr(names + 1, ',');cerr.write(names, comma - names) << " : " << arg1<<" | ";__f(comma+1, args...);
}
#else
#define trace(...)
#endif

typedef struct vertex  //for each customer 
{
    pair<int,int> c; //coordinates 
    int e,l,d,mul; //earliest time, latest time, demand
    int fill;
}vertex;

typedef struct node  // node for beam search
{
    vector<vertex> vis;  //list of visited nodes
    vector<vertex> nonvis; //list of not visited nodes
    float dist; //total distance up till now
    float error;
}node;

float heuristic(float dist, float error)
{
    return dist + error;
}

bool cmpp(node a,node b)  //compare function
{
    return a.vis.back().e<b.vis.back().e;
}
vertex depot;
bool impossible;

vertex create_vertex(int x,int y,int e,int l,int d, int m)
{
    vertex v;
    v.c = MP(x,y);
    v.e = e;
    v.l = l;
    v.d = d;
    v.mul = m;
    return v;
}

vector<vector<pair<int,pair<int,int>>>> kmeans(vector<pair<int,pair<int,int>>> customers,int m,int cmax)
{
    int n=SZ(customers);
    bool found=false;
    int maxct=0;
    while(found==false)
    {
        maxct++;
        bool move=true;
        vector<pair<int,pair<int,int>>> tmp,N;
        fr(i,0,SZ(customers)) N.PB(customers[i]);
        vector<pair<float,float>> centroids;
        vector<vector<pair<int,pair<int,int>>>> clusters;
        fr(i,0,m) clusters.PB(tmp);
        random_shuffle(N.begin(),N.end());
        fr(i,0,m) centroids.PB(MP(N[i].second.first,N[i].second.second));        
        fr(i,0,n)
        {
            int x,y,mincx,mincy,minc=0;
            float mindist;
            x=N[i].second.first;
            y=N[i].second.second;
            mincx=centroids[0].first;mincy=centroids[0].second;
            mindist=sqrt((x-mincx)*(x-mincx)+(y-mincy)*(y-mincy));            
            fr(j,0,m)
            {
                int a=centroids[j].first;
                int b=centroids[j].second;
                float dist=sqrt((x-a)*(x-a)+(y-b)*(y-b));
                if(dist<mindist)
                {
                    mincx=a;mincy=b;
                    mindist=dist;
                    minc=j;
                }
            }
            clusters[minc].PB(MP(N[i].first,MP(x,y)));
        }
        int lim=0;
        while(move==true)
        {
            vector<vector<pair<int,pair<int,int>>>> newcluster;
            vector<pair<int,pair<int,int>>> ins;
            fr(i,0,m) newcluster.PB(ins);
            lim++;
            fr(i,0,m)
            {
                int newx=0,newy=0;
                fr(j,0,SZ(clusters[i]))
                {
                    newx+=clusters[i][j].second.first;
                    newy+=clusters[i][j].second.second;
                }
                centroids[i]=MP(newx/SZ(clusters[i]),newy/SZ(clusters[i]));
            }
            int num_change=0;
            fr(i,0,m) fr(k,0,SZ(clusters[i]))
            {
                int x,y,mincx,mincy,minc=0;
                float mindist;
                x=clusters[i][k].second.first;
                y=clusters[i][k].second.second;
                mincx=centroids[0].first;mincy=centroids[0].second;
                mindist=sqrt((x-mincx)*(x-mincx)+(y-mincy)*(y-mincy));

                fr(j,0,m)
                {
                    int a=centroids[j].first;
                    int b=centroids[j].second;
                    float dist=sqrt((x-a)*(x-a)+(y-b)*(y-b));
                    if(dist<mindist)
                    {
                        mincx=a;mincy=b;
                        mindist=dist;
                        minc=j;
                    }
                }
                newcluster[minc].PB(MP(clusters[i][k].first,MP(x,y)));
                if(i!=minc) num_change++;
            }
            clusters.clear();
            clusters=newcluster;
            if(num_change==0) move=false;
            if(lim>=1000)
            {
               impossible = true;
               move=false;
            }
        }

        int check=0;
        fr(i,0,m)
        {
            int cap=0;
            fr(j,0,SZ(clusters[i])) cap+=clusters[i][j].first;
            if(cap>cmax) check=1;
        }
        if(check==0) return clusters;
        if(maxct>1000)
        {
            impossible = true;
            return clusters;
        }
    }
}

float dist(vertex v1,vertex v2)
{
    float x=v1.c.first-v2.c.first;
    float y=v1.c.second-v2.c.second;
    return sqrt(x*x + y*y);
}

node beam_search(int beam_width,vector<vertex> c)
{
    float error_limit = 100;
    node best;
    node root;
    root.vis.PB(depot);
    root.nonvis=c;
    root.dist=0;
    root.error=0;
    vector<node> B;
    B.PB(root);
    best=root;
    best.dist=INF;
    best.error=INF;
    while(B.size()!=0)
    {
        vector<node> Boff;
        for(int i=0;i<(int)B.size();i++)
        {
            for(int j=0;j<(int)B[i].nonvis.size();j++)
            {
                float dt=dist(B[i].vis.back(),B[i].nonvis[j]);
                float error = min(fabs(dt + B[i].dist - B[i].nonvis[j].e), fabs(dt + B[i].dist - B[i].nonvis[j].l));
                if(dt+ B[i].dist>=B[i].nonvis[j].e && dt + B[i].dist<=B[i].nonvis[j].l) 
                    error = 0;
                error = B[i].nonvis[j].mul * error;
                if(error > error_limit)
                    continue;
                node temp=B[i];
                temp.vis.PB(B[i].nonvis[j]);
                vector<vertex> tmp;
                for(int k=0;k<(int)B[i].nonvis.size();k++)
                    if(k!=j)
                        tmp.PB(B[i].nonvis[k]);
                temp.nonvis=tmp;
                temp.dist=dt+B[i].dist;
                temp.error += error;
                if(temp.nonvis.size()==0)
                {
                    temp.dist=dist(temp.vis.back(),depot) + temp.dist;
                    temp.vis.PB(depot);
                    if(temp.dist >= depot.e  && temp.dist <= depot.l && heuristic(temp.dist, temp.error) <heuristic(best.dist, best.error))
                        best=temp;
                }
                else
                    Boff.PB(temp);
            }
        }
        sort(Boff.begin(),Boff.end(),cmpp);
        B.clear();
        for(int i=0;i<(int)beam_width && i< (int)Boff.size();i++)
            B.PB(Boff[i]);
    }
    if(best.dist==INF)
        impossible=true;
    return best;
}

int get_input(string s)
{
    int inp;
    cout<<s<<"\n";
    scan(inp);
    return inp;
}

struct cmp{
	bool operator()(const vertex lhs, const vertex rhs) const {
		if( lhs.c.first < rhs.c.first)
            return 1;
        else if(lhs.c.second < rhs.c.second)
            return 1;
        else if(lhs.e < rhs.e)
            return 1;
        else if(lhs.l < rhs.l)
            return 1;
        else if(lhs.mul < rhs.mul)
            return 1;
        else
            return 0;
	}
};

pair<float,pair<float,float> > MainFunc(int n,int M,int cmax,map<vertex,vector<vertex>,cmp> CusDep,string cas)
{
	int cnt=0;
	pair<float,pair<float,float>> Fans;
	Fans.first= 0;
	Fans.second=MP(0,0);
	for(auto it=CusDep.begin();it!=CusDep.end();it++,++cnt)
	{
    	vector<pair<int,pair<int,int>>> customers;  //customer list for kmeans
    	map<pair<int,int>,pair<int,int>> tim;  //dictionary to store earlist time and latest time based on customer locations
    	map<pair<int,int>,int> cust_mul; // dictionary to store multiplier
    	srand(time(NULL));
    	map<pair<int,int>,int> done; 
		//to check no 2 customer locations are same
		vertex TempDep=it->first;
		vector<vertex > TempCust=it->second;
    	fr(i,0,TempCust.size())
    	{

			int x,y,e,l,z,mul;
        	x=TempCust[i].c.first-TempDep.c.first;
			y=TempCust[i].c.second-TempDep.c.second;
			e=TempCust[i].e;
			l=TempCust[i].l;
			z=TempCust[i].d;
			mul=TempCust[i].mul;
        	if(!done[MP(x,y)]) 
        	{
            	customers.PB(MP(z,MP(x,y)));  //add to customers list
            	tim[MP(x,y)]=MP(e,l); //store times based on customer locations
            	cust_mul[MP(x,y)] = mul;
            	done[MP(x,y)]=1;
        	}
    	}
    	int scn;  
		pair<float,pair<float,float> > best;
        vector<node> bestpath;
		best.second.second=INF;
		best.second.first=0;
		best.first=0;
		scn=it->first.l;
    	for(int m=1;m< M+2;m+=M/3)  //iterating over number of vehicles used
    	{
			if( m >SZ(customers) ) break;
        	impossible=false;
        	//printf("For number of vehicles %d : \n",m);
        	vector<vector<pair<int,pair<int,int>>>> clusters=kmeans(customers,m,cmax);
        	vector<vector<vertex>> clstr;  //cluster list of vectors to be used in beam search
        	if(impossible==true)  // if no solution exists
        	{
            	//printf("Clusters cannot be formed : test case not possible \n");
            	continue;
        	}
        	//printf("Clusters formed:\n");
        	fr(i,0,SZ(clusters))
        	{
            	//printf("%d: ",i+1);
            	vector<vertex> tp;
            	fr(j,0,SZ(clusters[i]))
            	{
                	pair<int,int> coor=clusters[i][j].second;
                	int d=clusters[i][j].first;
                	pair<int,int> ti=tim[coor];
                	int mul = cust_mul[coor];
                	vertex pu = create_vertex(coor.first, coor.second, ti.first, ti.second, d, mul);
                	//printf(" (%d %d)",coor.first,coor.second);
                	tp.PB(pu);
            	}
            	clstr.PB(tp);
            	//printf("\n");
        	}
        	depot = create_vertex(0, 0, 0, scn, 0, 0); //create depot vertex
        	vector<node>path;  //final path of nodes
       	 	//printf("Routes found by beam search \n");
        	float tdist=0, terror=0; // total distance and total error of all paths
        	fr(i,0,SZ(clstr))
        	{
            	path.PB(beam_search(10,clstr[i]));   // beam search on each cluster
            	if(impossible==true)
            	{
                	//printf("Solution cannot be formed : test case not possible \n");  // of no solution found using beam search
                break;
            	}
            //	printf("Cluster: ");
            	//print(i+1);
            	//fr(j,0,SZ(path[i].vis)) printf("(%d %d) ",path[i].vis[j].c.first,path[i].vis[j].c.second);  // print path found using beam search
            	//printf("\nDist : %f\n",path[i].dist);
            	tdist+=path[i].dist;
        	}
        	if(impossible==true) continue;
        		//printf("Total distance : %f\n",tdist);
        	//printf("Routes found after local search \n");
        	tdist = 0;
        	fr(i,0,SZ(clstr))   // local search to improve the solution
        	{
            	vector<vertex> rt = path[i].vis;
            	vector<vertex> nw;
            	float tot = path[i].dist, terr = path[i].error;
            	bool imp = true;
				int lim=0;
				int LIM=0;
            	while(imp == true && lim <LIM)
            	{
					lim++;
                	imp = false;
                	int ct=0;
                	fr(it,0,SZ(rt)-1) fr(j,it+2,SZ(rt)-1)
                	{
                    	vertex a = rt[it];
                    	vertex b = rt[j];
                    	vertex c = rt[it+1];
                    	vertex d = rt[j+1];
                    	ct++;
                    	fr(x,0,it+1) nw.PB(rt[x]);
                    	nw.PB(b);
                    	for(int x=j-1;x>=it+1;x--) nw.PB(rt[x]);
                    	nw.PB(d);
                    	fr(x,j+2,SZ(rt)) nw.PB(rt[x]);
                    	int status = 1, dt=0, err = 0;
                    	fr(k,0,SZ(nw))
                    	{
                        	vertex vt = nw[k];
                        	if(k!=0) dt += dist(nw[k], nw[k-1]);
                        	float cur_tim = dt, error, limit=100;  // multiply by speed
                        	if(cur_tim<vt.e || cur_tim>vt.l) 
                            error = min(fabs(cur_tim-vt.e), fabs(cur_tim-vt.l));
                        	else error = 0;
                        	if(error > limit)
                        	{
                            	status = 0;
                            	break;
                        	}
                        	err += error;
                    	}
                    	if(heuristic(dt, err) > heuristic(tot, terr))
                    	{
                        	status = 0;
                        	break;
                    	}
                    	if(status==1)
                    	{
                        	rt=nw;
                        	nw.clear();
                        	tot = tot-dist(a,c)-dist(b,d) + dist(a,b) + dist(c,d);
                        	terr = err;
                        	imp=true;
                    	}
                	}
            	}
            	path[i].vis=rt;
            	path[i].dist=tot;
            	path[i].error = terr;
            	//printf("Cluster: ");
            //	print(i+1);
            //	fr(j,0,SZ(path[i].vis)) printf("(%d %d) ",path[i].vis[j].c.first,path[i].vis[j].c.second);  // improved solution
            //	printf("Dist : %f\n",tot);
            //	printf("Error : %f\n",terr);
            	tdist = tdist + tot;
            	terror += terr;
        	}
        //	printf("Total distance  for Depot %d : %f\n",cnt,tdist);

        //	printf("Total Error : %f\n",terror);
			if(	best.second.first +best.second.second  > tdist + terror)
			{
				best.second.first=tdist;
				best.second.second=terror;
				best.first=m;
                bestpath = path;
			}

        //	printf("--------------------------------------------\n");
		}
		cout<<"For Depot " <<cnt  <<": Best solution " << "\n";
		cout<< " Number of Vehicles : " <<best.first << endl;
        printf("Final Routes found: ");
        for(int itt=0;itt<bestpath.size();itt++){
            for(int jt=0;jt<bestpath[itt].vis.size();jt++){
                printf("(%d %d) ",bestpath[itt].vis[jt].c.first + it->first.c.first,bestpath[itt].vis[jt].c.second + it->first.c.second);
            }
            printf("\n");
        }
        printf("-------------------------\n");
		Fans.first+=best.first;
        if(best.second.first <= 0) best.second.first = 5000;
		Fans.second.first+=best.second.first;
		Fans.second.second+=best.second.second;
	}
	return Fans;
}

vertex EarAndLat;
vertex Lat;
vertex Prior;

bool CompareCustomersEL(const  vertex &c1, const  vertex &c2)
{
	if(c1.e<c2.e)
		return 1;
	else if(c1.e==c2.e)
		return c1.l<c2.l;
	else
		return 0;
}
bool CompareCustomersPR(const vertex &c1,const vertex &c2)
{
	return c1.mul>c2.mul;

}
bool CompareCustomersL(const vertex &c1,const vertex &c2)
{
	return c1.l<c2.l;
}
bool CompareCustomerDepotEL(const vertex &d1,const vertex &d2)
{
	return dist(EarAndLat,d1) <dist(EarAndLat,d2);
}
bool CompareCustomerDepotL(const vertex &d1,const vertex &d2)
{
	return dist(Lat,d1) < dist(Lat,d2);
}
bool CompareCustomerDepotPR(const vertex &d1,const vertex &d2)
{
	return dist(Prior,d1) < dist(Prior,d2);
}
vector<vertex> Depots;
vector<vertex> Customers;
map<vertex,vector<vertex>,cmp > EarliestAndLatest()
{
	sort(Customers.begin(),Customers.end(),CompareCustomersEL);
    vector<vertex> tmpdep;

	for(int i=0;i<Depots.size();i++)
	{
        Depots[i].fill=0;
        tmpdep.push_back(Depots[i]);
    }

	map<vertex,vector<vertex>, cmp> Assign;

	for(int i=0;i<Customers.size();i++)
	{
		EarAndLat =Customers[i]; 
        sort(Depots.begin(),Depots.end(),CompareCustomerDepotEL);
        sort(tmpdep.begin(),tmpdep.end(),CompareCustomerDepotEL);
        for(int j=0;j<Depots.size();j++)
        {
            if(Depots[j].fill + EarAndLat.d <=Depots[j].d)
            {
                Depots[j].fill +=EarAndLat.d;
                Assign[tmpdep[j]].PB(Customers[i]);
                break;
            }
        }
	}
    // printf("%d\n", Depots.size());
    // printf(")))))))))))))))))))))))))))))))))))))))))))\n");
    // for(auto it = Assign.begin(); it!= Assign.end();it++){ //eldmulfill
    //     printf("%d %d %d %d %d %d %d\n", it->first.c.first, it->first.c.second, it->first.e, it->first.l, it->first.d, it->first.mul, it->first.fill);
    //     printf("-----=====\n");
    //     for(int i=0;i<it->second.size();i++){
    //         printf("%d %d))\n", it->second[i].c.first, it->second[i].c.second);
    //     }
    // }
	return Assign;
}
map<vertex,vector<vertex>,cmp > Latest()
{

	sort(Customers.begin(),Customers.end(),CompareCustomersL);


	vector<vertex> tmpdep;

    for(int i=0;i<Depots.size();i++)
    {
        Depots[i].fill=0;
        tmpdep.push_back(Depots[i]);
    }
	map<vertex,vector<vertex>,cmp> Assign;

	for(int i=0;i<Customers.size();i++)
	{
		Lat =Customers[i]; 
		sort(Depots.begin(),Depots.end(),CompareCustomerDepotL);
        sort(tmpdep.begin(),tmpdep.end(),CompareCustomerDepotL);

		for(int j=0;j<Depots.size();j++)
		{
			if(Depots[j].fill + Lat.d <=Depots[j].d)
			{
				Depots[j].fill +=Lat.d;
				Assign[tmpdep[j]].PB(Customers[i]);
				break;
			}
		}
	}
	return Assign;
}

map<vertex,vector<vertex>,cmp > Priority()
{

	sort(Customers.begin(),Customers.end(),CompareCustomersPR);

	vector<vertex> tmpdep;

    for(int i=0;i<Depots.size();i++)
    {
        Depots[i].fill=0;
        tmpdep.push_back(Depots[i]);
    }
    map<vertex,vector<vertex>,cmp> Assign;


	for(int i=0;i<Customers.size();i++)
	{
		Prior=Customers[i];
		sort(Depots.begin(),Depots.end(),CompareCustomerDepotPR);
        sort(tmpdep.begin(),tmpdep.end(),CompareCustomerDepotPR);

		for(int j=0;j<Depots.size();j++)
		{
			if(Depots[j].fill + Prior.d <=Depots[j].d)
			{
				Depots[j].fill +=Prior.d;
				Assign[tmpdep[j]].PB(Customers[i]);
				break;
			}
		}
	}
	return Assign;
}

int main(int argc, char *argv[])
{
	int NumberOfDepots;
	cout<< "Enter Number of Depots :" ;
	cin >> NumberOfDepots;
	for(int i=0;i<NumberOfDepots;i++)
	{
		int x,y;
		vertex TempDep;
		cout<<"Enter X and Y Coordinates of Depots : ";
		cin>>x>>y;
		TempDep.c=MP(x,y);
		int d;
		cout<<"Enter Demand of Depot : ";
		cin>>d;
		TempDep.d=d;

		TempDep.e=0;
		TempDep.l=INF;
		Depots.PB(TempDep);

	}
	int NumberOfCustomers;
	cout<< "Enter Number of Customers : ";
	cin >> NumberOfCustomers;
	for(int i=0;i<NumberOfCustomers;i++)
	{
		int x,y;
		vertex TempCust;
		cout<<"Enter X and Y Coordinates of Customer : ";
		cin>>x>>y;
		TempCust.c=MP(x,y);
		int e,l;
		cout<<"Enter Earliest Time and Latest Time of Customer ";
		cin>>e>>l;
		TempCust.e=e;
		TempCust.l=l;

		int d;
		cout<<"Enter Customer Demand : ";
		cin>>d;
		TempCust.d=d;

		int mul;
		cout<<"Enter Priority of Customer :  ";
		cin>>mul;
		TempCust.mul=mul;

		Customers.PB(TempCust);
	}
	int CapacityOfVehicles = 1000;
	
	pair<float,pair<float,float> > ans1,ans2,ans3;
    vector<vector<float>> distances;
    int errlim = 5000;
    for(int tim = errlim;tim<errlim+1;tim+=10000){
        printf("\n*****ERROR LIMIT %d\n\n", tim/10);
        for(int i=0;i<Customers.size();i++){
            Customers[i].e = max(0, Customers[i].e - tim * 10);
            Customers[i].l = Customers[i].e + tim * 10;
        }
        float aa, bb, cc;
    	ans1=MainFunc(NumberOfCustomers,9,CapacityOfVehicles,EarliestAndLatest(),"EarliestAndLatest");
    	



    	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    	cout<< "Earliest and Latest \n";
    	cout<< "---------------------------------------\n";
    	cout<<"Total Number of Vehicles :" << ans1.first <<endl;
    	cout<<"Total Distance :" << ans1.second.first<<endl;
        aa = ans1.second.first;
       // cout<<"Total error :" << ans1.second.second<<endl;

    	cout<< "---------------------------------------\n";

        printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");

    	ans2=MainFunc(NumberOfCustomers,9,CapacityOfVehicles,Priority(),"Priority");

    	cout<< "Priority \n";
    	cout<< "---------------------------------------\n";
    	cout<<"Total Number of Vehicles : " << ans2.first<<endl;;
    	cout<<"Total Distance :" << ans2.second.first<<endl;
        bb = ans2.second.first;
      //  cout<<"Total error :" << ans1.second.second<<endl;
    	cout<< "---------------------------------------\n";

        printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");

        ans3=MainFunc(NumberOfCustomers,9,CapacityOfVehicles,Latest(),"Latest");

    	
    	cout<< "Latest \n";
    	cout<< "---------------------------------------\n";
    	cout<<"Total Number of Vehicles : " << ans3.first<<endl;
    	cout<<"Total Distance :" << ans3.second.first<<endl;
        cc = ans3.second.first;
      //  cout<<"Total error :" << ans1.second.second<<endl;
    	cout<< "---------------------------------------\n";
        cout<<aa<<" || "<<bb<<" || "<<cc<<"\n";
    }
   // for(int i=0;i<distances.size();i++){
     //   printf("%f %f %f\n");
    //}
	return 0;
}
