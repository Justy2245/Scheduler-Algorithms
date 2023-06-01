#include <iostream>
#include <fstream>
#include <queue>
using namespace std;

// structure used to hold the process information
struct Node
{
    string pID;
    int burst;
    int arrival;
};
// not used directly in program more so to help with debugging
// by checking what is in the queue
void displayQueue(queue<Node> display)
{
    queue<Node> tempNode = display;
    while (!tempNode.empty())
    {
        cout << tempNode.front().pID;
        cout << "\t" << tempNode.front().burst;
        cout << "\t" << tempNode.front().arrival;
        cout << "\n";
        tempNode.pop();
    }
}

int main()
{
    queue<Node> jobQueue;
    queue<Node> readyQueue;
    queue<Node> tempJobQueue;
    queue<Node> tempQueue;
    queue<int> turnaround;
    queue<Node> turnaroundRR;
    queue<Node> turnaroundSRTF;
    queue<int> waiting;
    Node shortest;
    int time = 0;
    int turnAvg = 0;
    int waitAvg = 0;
    int quantum = 3;
    int tracker = 0;
    ifstream inFile;

    // open the file to read in
    inFile.open("input.txt", ios::in);
    if(!inFile)
    {
        cout << "There is no file with that name";
        exit(0);
    }
    // Initializing job queue
    while(!inFile.eof())
    {
        string temp;
        int temp1, temp2;
        inFile >> temp >> temp1 >> temp2;
        if(temp == "")
        {
            break;
        }
        Node tempNode;
        tempNode.pID = temp;
        tempNode.burst = temp1;
        tempNode.arrival = temp2;
        jobQueue.push(tempNode);
    }
    // FCFS Scheduling Algorithm
    tempJobQueue = jobQueue;
    cout << "FCFS Scheduling\n";
    while(!tempJobQueue.empty())
    {
        // add all processes to the queue based on arrival time
        while(tempJobQueue.front().arrival <= time && !tempJobQueue.empty())
        {
            readyQueue.push(tempJobQueue.front());
            int check = tempJobQueue.front().arrival;
            tempJobQueue.pop();
            while(tempJobQueue.front().arrival == check)
            {
                readyQueue.push(tempJobQueue.front());
                check = tempJobQueue.front().arrival;
                tempJobQueue.pop();
            }
        }
        // loop through ready queue in order until it is empty
        while(!readyQueue.empty())
        {
            cout << time << " ";
            cout << readyQueue.front().pID;
            cout << "\tProcess Terminated\n";
            time = time + readyQueue.front().burst;
            int turnaroundTemp = time - readyQueue.front().arrival;
            turnaround.push(turnaroundTemp);
            waiting.push(turnaroundTemp - readyQueue.front().burst);
            readyQueue.pop();
        }
    }
    // Summary Statistics
    cout << time << "\tComplete";
    cout << "\n\nProcess\tTurnaround      Waiting\n";
    cout << "ID\tTime\t        Time\n";
    for(char n = 'A'; n < 'I'; n++)
    {
        cout << n << "\t" << turnaround.front() << "\t        " << waiting.front() << "\n";
        turnAvg += turnaround.front();
        waitAvg += waiting.front();
        turnaround.pop();
        waiting.pop();
    }
    cout << "Average\t" << turnAvg << "/8 = " << turnAvg / 8;
    cout << "\t" << waitAvg << "/8 = " << waitAvg / 8 << "\n";

    // Round Robin Scheduling Algorithm
    cout << "\nRound Robin Scheduling\n";
    time = 0;
    tempJobQueue = jobQueue;
    while(!tempJobQueue.empty() || !readyQueue.empty() || !tempQueue.empty())
    {
        // add all processes to the queue based on arrival time
        while(tempJobQueue.front().arrival <= time && !tempJobQueue.empty())
        {
            readyQueue.push(tempJobQueue.front());
            int check = tempJobQueue.front().arrival;
            tempJobQueue.pop();
            while(tempJobQueue.front().arrival == check)
            {
                readyQueue.push(tempJobQueue.front());
                check = tempJobQueue.front().arrival;
                tempJobQueue.pop();
            }
        }
        int temp = readyQueue.size();
        // put all of temporary queue onto the end of the
        // ready queue; more explanation on tempqueue below
        while(!tempQueue.empty())
        {
            readyQueue.push(tempQueue.front());
            tempQueue.pop();
        }
        for(int n = 0; n < temp; n++)
        {
            cout << time << " ";
            cout << readyQueue.front().pID;
            if(readyQueue.front().burst - quantum <= 0)
            {
                cout << "\tProcess Terminated\n";
                time = time + readyQueue.front().burst;
                // Temporarily using the burst time field to hold the finish time or process
                // to calculate turnaround time later.
                turnaroundRR.push(readyQueue.front());
                turnaroundRR.back().burst = time;
                readyQueue.pop();
            }
            else
            {
                time = time + 3;
                readyQueue.front().burst -= quantum;
                cout << "\tQuantum Expired - " << readyQueue.front().burst << " ms remaining\n";
                // tempqueue used to keep processes in order otherwise processes that have not
                // finished due to long burst time will be out of order if put directly
                // back into ready queue
                tempQueue.push(readyQueue.front());
                readyQueue.pop();
            }
        }
    }
    // Summary Statistics
    cout << time << "\tComplete";
    turnAvg = 0;
    waitAvg = 0;
    tempQueue = jobQueue;
    cout << "\n\nProcess\tTurnaround      Waiting\n";
    cout << "ID\tTime\t        Time\n";
    for(char n = 'A'; n < 'I'; n++)
    {
        cout << n << "\t";
        bool loop = true;
        while(loop == true)
        {
            // cycling through queue until matching pID is found so that summary is in alphabetical order
            if(turnaroundRR.front().pID[0] == n)
            {
                // burst field is holding the exit time from earlier
                int turnaroundTemp = turnaroundRR.front().burst - turnaroundRR.front().arrival;
                int waitingTemp = turnaroundTemp - tempQueue.front().burst;
                turnAvg += turnaroundTemp;
                waitAvg += waitingTemp;
                tempQueue.pop();
                cout << turnaroundTemp << "\t        ";
                cout << waitingTemp << "\n";
                loop = false;
            }
            else
            {
                turnaroundRR.push(turnaroundRR.front());
                turnaroundRR.pop();
            }
        }
    }
    cout << "Average\t" << turnAvg << "/8 = " << turnAvg / 8;
    cout << "\t" << waitAvg << "/8 = " << waitAvg / 8 << "\n";

    // SRTF Scheduling Algorithm
    cout << "\nSRTF Scheduling\n";
    time = 0;
    tracker = 0;
    tempJobQueue = jobQueue;
    while(!tempJobQueue.empty() || !readyQueue.empty())
    {
        // add all processes to the queue based on arrival time
        while(tempJobQueue.front().arrival <= time && !tempJobQueue.empty())
        {
            readyQueue.push(tempJobQueue.front());
            int check = tempJobQueue.front().arrival;
            tempJobQueue.pop();
            while(tempJobQueue.front().arrival == check)
            {
                readyQueue.push(tempJobQueue.front());
                check = tempJobQueue.front().arrival;
                tempJobQueue.pop();
            }
        }
        if(tempJobQueue.empty())
        {
            tracker = 10000;
        }
        else
        {
            tracker = tempJobQueue.front().arrival;
        }
        while(!readyQueue.empty())
        {
            // tracker keeps track if there are more processes arriving
            if(time >= tracker)
            {
                break;
            }
            else
            {
                int temp = readyQueue.size();
                shortest = readyQueue.front();
                // iterate through queue to find the shortest time
                for(int n = 0; n < temp; n++)
                {
                    if(readyQueue.front().burst < shortest.burst)
                    {
                        shortest = readyQueue.front();
                    }
                    else
                    {
                        readyQueue.push(readyQueue.front());
                        readyQueue.pop();
                    }
                }
                for(int n = 0; n < temp; n++)
                {
                    // move shortest process to front of queue
                    if(readyQueue.front().pID == shortest.pID)
                    {
                        // go here if there is still time before another process is added to the ready queue
                        if(tracker - readyQueue.front().burst >= 0)
                        {
                            cout << time << " ";
                            cout << readyQueue.front().pID << "\t Process Terminated\n";
                            time = time + readyQueue.front().burst;
                            tracker = tracker - readyQueue.front().burst;
                            // Temporarily using the burst time field to hold the finish time or process
                            // to calculate turnaround time later.
                            turnaroundSRTF.push(readyQueue.front());
                            turnaroundSRTF.back().burst = time;
                            readyQueue.pop();
                        }
                        else
                        {
                            // subtract time from process burst field to keep track of how much time left
                            readyQueue.front().burst -= tracker;
                            time = time + tracker;
                            cout << time - tracker << " ";
                            cout << readyQueue.front().pID << "\t Process Preempted - ";
                            cout << readyQueue.front().burst << " ms remaining\n";
                            break;
                        }
                    }
                    else
                    {
                        readyQueue.push(readyQueue.front());
                        readyQueue.pop();
                    }
                }
            }
        }
    }
    // Summary Statistics
    cout << time << "\t Complete";
    turnAvg = 0;
    waitAvg = 0;
    tempQueue = jobQueue;
    cout << "\n\nProcess\tTurnaround      Waiting\n";
    cout << "ID\tTime\t        Time\n";
    for(char n = 'A'; n < 'I'; n++)
    {
        cout << n << "\t";
        bool loop = true;
        while(loop == true)
        {
            if(turnaroundSRTF.front().pID[0] == n)
            {
                // burst field is holding the exit time from earlier
                int turnaroundTemp = turnaroundSRTF.front().burst - turnaroundSRTF.front().arrival;
                int waitingTemp = turnaroundTemp - tempQueue.front().burst;
                turnAvg += turnaroundTemp;
                waitAvg += waitingTemp;
                tempQueue.pop();
                cout << turnaroundTemp << "\t        ";
                cout << waitingTemp << "\n";
                loop = false;
            }
            else
            {
                turnaroundSRTF.push(turnaroundSRTF.front());
                turnaroundSRTF.pop();
            }
        }
    }
    cout << "Average\t" << turnAvg << "/8 = " << turnAvg / 8;
    cout << "\t" << waitAvg << "/8 = " << waitAvg / 8 << "\n";
}
