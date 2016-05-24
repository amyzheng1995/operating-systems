#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <map>
#include <algorithm>

using namespace std;

//convert string to integer
int convert(const std::string &s) {
	int value;
	stringstream ss(s);
	ss >> value;
	return value;
}

//reading in the input file and parse
//save to a vector
void readin(ifstream &myfile, vector<vector<int> > &all_proc){
	string line;
	vector<string> data;
	while (getline(myfile, line)){
		if (line[0] != '#' && line[0] != '<'){
			data.push_back(line);
		}
	}
	for (unsigned int i = 0; i < data.size(); i++){
		for (unsigned int j = 0; j < data[i].size(); j++){
			if (data[i][j] == '|')
				data[i][j] = ' ';
		}
		vector<int> l;
		stringstream ss(data[i]);
		string temp;
		int tmp;
		while (ss >> temp){
			tmp = convert(temp);
			l.push_back(tmp);
		}
		all_proc.push_back(l);
	}

}
//print the queues
void printq(vector<int> proc){
	cout << "[Q";
	for (unsigned int i = 0; i < proc.size(); i++){
		cout <<' ' << proc[i];
	}
	cout << ']'<< endl;

}

int index (vector <vector <int> > processes, int ind){
	for (unsigned int i = 0; i< processes.size(); i++){
		if (processes[i][0] == ind){
			return i;
		}
	}
	return -1;
}

//helper function to determine whether to switch two numbers
bool sort_nums(int a, int b){
	if (a > b){
		return true;
	}
	return false;
}

//sort for SJF
void sort(vector<int>& ids,vector<vector<int> > all_proc){
	for (unsigned int i=0;i<ids.size();i++){
		for (unsigned int j=i+1;j<ids.size();j++){
			if (all_proc[index(all_proc,ids[i])][2]>all_proc[index(all_proc,ids[j])][2]){
				int temp = ids[i];
				ids[i] = ids[j];
				ids[j] = temp;
			}
			else if(all_proc[index(all_proc,ids[i])][1] == all_proc[index(all_proc,ids[j])][2]){
				if (sort_nums(ids[i], ids[j]) == true){
					int temp = ids[i];
					ids[i] = ids[j];
					ids[j] = temp;
				}
			}
		}
	}
}

//sort for FCFS at the start
void sort_ids(vector<int>& ids){
	for (unsigned int i=0;i<ids.size();i++){
		for (unsigned int j=i+1;j<ids.size();j++){
			if (ids[i] >= ids[j]){
				int t = ids[i];
				ids[i] = ids[j];
				ids[j] = t;
			}
			
		}
	}
}


void fcfs(vector<vector<int> > all_proc, map<int, int> times,int t_cs, ofstream& output){
	//calculate total burst time and burst numbers
	int t_burst = 0;
	int num_burst = 0;
	for (unsigned int i = 0; i < all_proc.size(); i++){
		t_burst = t_burst+all_proc[i][2]*all_proc[i][3];
		num_burst = num_burst+ all_proc[i][3];
	}
	vector<vector<int> > copy(all_proc);
	int pres = 0;
	int sum;
	int t = 0;
	int CPU = 0;
	int context_switch = 0;
	int cpu_time = 0;
	int start_time = 0; 
	int temp_CPU = 0; //current CPU process
	bool cpu_empty = true; 
	vector <int> io;
	vector <int> ids;
	map <int, int > wait_time;
	map <int, int > arrive_time;
	all_proc = copy;
	vector <int> emptyq;
	for (unsigned int i = 0; i < all_proc.size(); i++){
		ids.push_back(all_proc[i][0]);
	}
	
	//sort the order of processes for FCFS at the start
	sort_ids(ids);
	cout << "time "<<t<<"ms: Simulator started for FCFS" <<endl;

	while (1){
		for (unsigned int i = 0; i < all_proc.size(); i++){
			if (all_proc[i][1] == t){
				cout << "time " << t << "ms: P" <<all_proc[i][0] << " arrived ";
				emptyq.push_back(all_proc[i][0]);
				printq(emptyq);
				arrive_time[all_proc[i][0]] = t;
			}
		}

		//cpu's process to io
		if (cpu_time == t && t!=0){
			cpu_empty = true;
			//if process is terminated
			if (all_proc[index(all_proc,CPU)][3] == 1){
				cout<<"time "<<t<<"ms: P"<<CPU<<" terminated ";
				printq(emptyq);
			}
			//if process needs to be rerun...
			else{
				cout << "time "<<t << "ms: P"<<CPU<<" completed its CPU burst ";
				printq(emptyq);
				io.push_back(CPU);
				cout << "time "<<t <<"ms: P"<< CPU<<" blocked on I/O ";
				printq(emptyq);
				times[CPU] = t+ all_proc[index(all_proc,CPU)][4];
				
			}
			cpu_time = 0;
			CPU = 0;
		}
		//io finished
		for (unsigned int i =0; i< io.size();i++){
			if (times[io[i]] == t){
				times[io[i]] = 0;
				all_proc[index(all_proc,io[i])][3]--;
				cout<<"time "<<t<<"ms: P"<<io[i]<<" completed I/O ";
				printq(emptyq);
				emptyq.push_back(io[i]);
				cout << "time " << t << "ms: P" <<io[i] << " arrived ";
				printq(emptyq);
				arrive_time[io[i]] = t;
				io.erase(io.begin()+i);
			}
		}


		//add process to the cpu
		if (CPU == 0 && cpu_empty == true && emptyq.size() != 0){
			start_time = t + t_cs;
			cpu_empty = false;
			context_switch++;
			temp_CPU = emptyq[0];
			wait_time[temp_CPU]+=(t-arrive_time[temp_CPU]);
			emptyq.erase(emptyq.begin());
		}
		
		//cpu is empty and process is ready to be added to cpu
		if (t == start_time && cpu_empty == false){
			CPU = temp_CPU;
			temp_CPU = 0;
			cpu_time = t + all_proc[index(all_proc, CPU)][2];
			cout << "time "<< t << "ms: P"<<CPU <<" started using the CPU ";
			printq(emptyq);
			
		}

		if (emptyq.size() == 0 && CPU ==0 && io.size()==0 && temp_CPU==0) break;
		t++;
	}


	cout << "time "<<t<<"ms: Simulator ended for FCFS"<< endl;
	output << "Algorithm FCFS" << endl;
	float avg_burst = t_burst/float(num_burst);
	output <<"-- average CPU burst time: "<< setprecision(2) << fixed  << avg_burst << " ms" << endl;
	
	//calculate average wait time
	sum = 0;
	for (unsigned int i=0; i < wait_time.size();i++){
		sum+= wait_time[i];
	}
	output <<"-- average wait time: "<< setprecision(2) << fixed  << sum/float(num_burst)<< " ms"<<endl;
	output<<"-- average turnaround time: "<< setprecision(2) << fixed  << sum/float(num_burst) + avg_burst + 9<< " ms"<<endl;
	output<<"-- total number of context switches: "<<context_switch<<endl;
	output << "-- total number of preemptions: " << pres <<endl;
}


void srt(vector<vector<int> > all_proc, map<int, int> times,int t_cs, ofstream& output){
	//calculate total burst time and burst numbers
	int t_burst = 0;
	int num_burst = 0;
	for (unsigned int i = 0; i < all_proc.size(); i++){
		t_burst = t_burst+all_proc[i][2]*all_proc[i][3];
		num_burst = num_burst+ all_proc[i][3];
	}
	vector<vector<int> > copy(all_proc);
	int pres = 0;
	int sum;
	int t = 0;
	int CPU = 0;
	int context_switch = 0;
	int start_time = 0; 
	bool preempted = false;
	int temp_CPU = 0; //current CPU process
	bool cpu_empty = true; 
	vector <int> io;
	vector <int> ids;
	map <int, int > wait_time;
	map <int, int> arrive_time;
	all_proc = copy;
	vector <int> emptyq;
	std::vector<int>::iterator it;
	vector <int> cpu_times;
	for (unsigned int i = 0; i < all_proc.size(); i++){
		wait_time[all_proc[i][0]] = 0;
		cpu_times.push_back(all_proc[i][2]);
	}
	


	cout << "time "<<t<<"ms: Simulator started for SRT" <<endl;

	while (1){
		for (unsigned int i = 0; i < all_proc.size(); i++){
			if (all_proc[i][1] == t){
				ids.push_back(all_proc[i][0]);
				cout << "time " << t << "ms: P" <<all_proc[i][0] << " arrived";
				emptyq.push_back(all_proc[i][0]);
				sort(ids,all_proc);
				arrive_time[all_proc[i][0]] = t;
				
				// wait_t.push_back(t-wait_time[temp_CPU]);
				if (ids.size() > 1 && ids[0] == all_proc[i][0] ){
					cout << ", preempting P" << ids[1]<<" ";
					emptyq.pop_back();
					printq(emptyq);
					preempted = true;
					pres++;
					arrive_time[ids[1]] = t;
					emptyq.erase(std::remove(emptyq.begin(), emptyq.end(), ids[1]), emptyq.end());
					it = emptyq.begin();
						it = emptyq.insert ( it , ids[0] );
						it = emptyq.begin()+1;
						it = emptyq.insert ( it , ids[1] );
				}
				else{
					cout << " ";
					printq(emptyq);
				}
			}
		}
		if (CPU != 0 && t > t_cs && cpu_times[index(all_proc, CPU)] == 0 && t != 0 ){
			cpu_empty = true;
			if (all_proc[index(all_proc,CPU)][3] == 1){
				cout<<"time "<<t<<"ms: P"<<CPU<<" terminated ";
				ids.erase(std::remove(ids.begin(), ids.end(), CPU), ids.end());
				printq(emptyq);
			}
			//if process needs to be rerun...
			else{
				cout << "time "<<t << "ms: P"<<CPU<<" completed its CPU burst ";
				ids.erase(std::remove(ids.begin(), ids.end(), CPU), ids.end());
				 printq(emptyq);
				io.push_back(CPU);
				cout << "time "<<t <<"ms: P"<< CPU<<" blocked on I/O ";
				printq(emptyq);
				times[CPU] = t+ all_proc[index(all_proc,CPU)][4];

				
			}
			CPU = 0;
		}

		//io finished
		for (unsigned int i =0; i< io.size();i++){
			if (times[io[i]] == t ){
				times[io[i]] = 0;
				all_proc[index(all_proc,io[i])][3]--;
				cout<<"time "<<t<<"ms: P"<<io[i]<<" completed I/O ";
				printq(emptyq);
				emptyq.push_back(io[i]);
				ids.push_back(io[i]);
				cout << "time " << t << "ms: P" <<io[i] << " arrived";
				
				sort(ids,all_proc);
				if (ids.size() > 1 &&  ids[0] == io[i]){
					cout << ", preempting P" << ids[1] <<" ";
					emptyq.pop_back();
					printq(emptyq);
					preempted = true;
					pres++;
					arrive_time[ids[1]] = t;

					emptyq.erase(std::remove(emptyq.begin(), emptyq.end(), ids[1]), emptyq.end());
						it = emptyq.begin();
						it = emptyq.insert ( it , ids[0] );
						it = emptyq.begin()+1;
						it = emptyq.insert ( it , ids[1] );
				}
				else{
					cout << " ";
					printq(emptyq);
				}
				arrive_time[io[i]] = t;
				cpu_times[index(all_proc, io[i])]=all_proc[index(all_proc,io[i])][2];
				io.erase(io.begin()+i);
			}
		}


		//add process to the cpu
		if ( (CPU == 0 && cpu_empty == true && emptyq.size() != 0) || (preempted == true)){
			start_time = t + t_cs;
			cpu_empty = false;
			context_switch++;
			temp_CPU = emptyq[0];
			wait_time[temp_CPU]+=(t-arrive_time[temp_CPU]);

			if (preempted){
				cpu_times[index(all_proc, emptyq[1])] = cpu_times[index(all_proc, emptyq[1])] + t_cs;
				preempted =false;
			}
			emptyq.erase(emptyq.begin());
		}
		//cpu is empty and process is ready to be added to cpu
		if (t == start_time && cpu_empty == false){
			CPU = temp_CPU;
			temp_CPU = 0;
			cout << "time "<< t << "ms: P"<<CPU <<" started using the CPU ";
			printq(emptyq);
			sort(ids,all_proc);
			
		}
		if (emptyq.size() == 0 && CPU ==0 && io.size()==0 && temp_CPU ==0) break;
		t++;
		if (CPU != 0 && t > t_cs) cpu_times[index(all_proc, CPU)]--;
	}


	cout << "time "<<t<<"ms: Simulator ended for SRT"<<endl;
	output << "Algorithm SRT" <<endl;


	float avg_burst = t_burst/float(context_switch);
	output <<"-- average CPU burst time: "<< setprecision(2) << fixed  << avg_burst << " ms" << endl;
	
	//calculate average wait time
	sum = 0;
	for (unsigned int i  = 0; i < all_proc.size(); i++){
		sum+=wait_time[all_proc[i][0]];
	}
	output <<"-- average wait time: "<< setprecision(2) << fixed  << sum/float(context_switch)<< " ms"<<endl;
	output<<"-- average turnaround time: "<< setprecision(2) << fixed  << sum/float(context_switch) + avg_burst + 9<< " ms"<<endl;
	output<<"-- total number of context switches: "<<context_switch<<endl;
	output << "-- total number of preemptions: " <<pres <<endl;

}

void rr(vector<vector<int> > all_proc, map<int, int> times,int t_cs, ofstream& output){
	//calculate total burst time and burst numbers
	int t_burst = 0;
	int num_burst = 0;
	for (unsigned int i = 0; i < all_proc.size(); i++){
		t_burst = t_burst + all_proc[i][2] * all_proc[i][3];
		num_burst = num_burst + all_proc[i][3];
	}
	int pres = 0;
	vector<vector<int> > copy(all_proc);
	int sum;
	int t = 0;
	int CPU = 0;
	int context_switch = 0;
	int start_time = 0; 
	int temp_CPU = 0; //current CPU process
	bool cpu_empty = true; 
	vector <int> io;
	vector <int> ids;
	map <int, int > wait_time;
	map<int, int > arrive_time;
	vector < int > ts;
	vector <int> cpu_times;
	all_proc = copy;
	vector <int> emptyq;
	for (unsigned int i = 0; i < all_proc.size(); i++){
		wait_time[all_proc[i][0]] = 0;
		ts.push_back(9);
		cpu_times.push_back(all_proc[i][2]);
	}
	int t_slice = 80;
	int time_count  = t_slice+t_cs;

	cout << "time "<<t<<"ms: Simulator started for RR (t_slice is "<< t_slice << "ms)" <<endl;

	while (1){
		//go through every process in the processes and if the arrival time is equal to 
		//current time, add it to the queue
		for (unsigned int i = 0; i < all_proc.size(); i++){
			if (all_proc[i][1] == t){
				cout << "time " << t << "ms: P" <<all_proc[i][0] << " arrived ";
				emptyq.push_back(all_proc[i][0]);
				printq(emptyq);
				arrive_time[all_proc[i][0]] = t;
			}
		}
		if (t > t_cs && CPU != 0 && t != 0 && ((time_count == 0  && cpu_times[index(all_proc, CPU)] == 0) || cpu_times[index(all_proc, CPU)] ==0 )) {
			cpu_empty = true;
			time_count = t_slice + t_cs;
			if (all_proc[index(all_proc,CPU)][3] == 1){
				cout<<"time "<<t<<"ms: P"<<CPU<<" terminated ";
				printq(emptyq);
			}
			
			//if process needs to be rerun...
			else{
				cout << "time "<<t << "ms: P"<<CPU<<" completed its CPU burst ";
				printq(emptyq);
				io.push_back(CPU);
				cout << "time "<<t <<"ms: P"<< CPU<<" blocked on I/O ";
				printq(emptyq);
				times[CPU] = t+ all_proc[index(all_proc,CPU)][4];
				
			}
			CPU = 0;
		}
		
		if(t > t_cs && CPU != 0 && t!=0 && cpu_times[index(all_proc, CPU)] > 0 && time_count == 0){
			int temp = all_proc[index(all_proc, CPU)][0];
			if (emptyq.size() == 0 || emptyq[0] == temp){
				arrive_time[CPU] = t;
				time_count = t_slice;
				emptyq.push_back(CPU);
				if (emptyq[0] == temp)
					emptyq.pop_back();
			}
			else{
				cout << "time " << t << "ms: Time slice expired, preempting P" << CPU <<" ";
				printq(emptyq);
				cpu_times[index(all_proc, CPU)]+=t_cs;
				pres++;
				arrive_time[CPU] = t;
				emptyq.push_back(CPU);	
				cpu_empty = true;
				time_count = t_slice+t_cs;
			}

		}
			
		//io finished
		for (unsigned int i =0; i< io.size();i++){
			if (times[io[i]] == t){
				times[io[i]] = 0;
				all_proc[index(all_proc,io[i])][3]--;
				cout<<"time "<<t<<"ms: P"<<io[i]<<" completed I/O ";
				printq(emptyq);
				emptyq.push_back(io[i]);
				cout << "time " << t << "ms: P" <<io[i] << " arrived ";
				printq(emptyq);
				arrive_time[io[i]] =t;

				cpu_times[index(all_proc, io[i])]=all_proc[index(all_proc,io[i])][2];
				io.erase(io.begin()+i);
				

			}
		}


		//add process to the cpu
		if (cpu_empty == true && emptyq.size() != 0){
			start_time = t + t_cs; //t_cs = 9
			cpu_empty = false; //there is something in cpu 
			context_switch++;
			temp_CPU = emptyq[0]; 
			wait_time[temp_CPU]+=(t-arrive_time[temp_CPU]);
			emptyq.erase(emptyq.begin()); //erase from the queue
		}

		//cpu is empty and process is ready to be added to cpu
		if (t == start_time && cpu_empty == false){
			CPU = temp_CPU;
			temp_CPU = 0; 
			cout << "time "<< t << "ms: P"<<CPU <<" started using the CPU ";
			printq(emptyq);
			time_count = 80;
		}
		if (emptyq.size() == 0 && CPU == 0 && io.size() == 0 && temp_CPU == 0) 
			break;
		t++;
		time_count--;
		if (t > t_cs && CPU != 0 )cpu_times[index(all_proc, CPU)]--;
	
	}
	

	cout << "time "<<t<<"ms: Simulator ended for RR"<< endl;
	output << "Algorithm RR" << endl;
	

	float avg_burst = t_burst/float(context_switch);
	output <<"-- average CPU burst time: "<< setprecision(2) << fixed  << avg_burst << " ms" << endl;
	
	//calculate average wait time
	sum = 0;
	for (unsigned int i  = 0; i < all_proc.size(); i++){
		sum+=wait_time[all_proc[i][0]];
	}
	output <<"-- average wait time: "<< setprecision(2) << fixed  << sum/float(context_switch)<< " ms"<<endl;
	output<<"-- average turnaround time: "<< setprecision(2) << fixed  << sum/float(context_switch) + avg_burst + 9<< " ms"<<endl;
	output<<"-- total number of context switches: "<<context_switch<<endl;
	output << "-- total number of preemptions: " << pres <<endl;
}


int main(int argc, char* argv[]){
	if (argc != 3){
		cerr << "Invalid number of arguments" << endl;
	}
	ifstream myfile(argv[1]);
	if (!myfile){
		cerr <<"file can't be read"<<endl;
		return 1;
	}
	ofstream output;
	if (!output){
		cerr <<"file can't be created"<<endl;
		return 1;
	}
	output.open(argv[2]);
	vector<vector<int> > all_proc;
	readin(myfile, all_proc);
	int t_cs = 9;
	map<int, int> times;
	fcfs(all_proc, times, t_cs, output);
	cout << endl;
	srt(all_proc, times, t_cs, output);
	cout << endl;
	rr(all_proc, times, t_cs, output);
	myfile.close();
}
