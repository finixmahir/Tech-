#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include <cmath>
using namespace std;

map<string, string> memoryDB;
string memoryFile = "memory.txt";
string hiddenFile = "m!_memory.txt";

// Lowercase helper
string lower(string s)
{
    for (auto &c : s)
        c = tolower(c);
    return s;
}

// Load saved memory
void loadMemory()
{
    ifstream file(memoryFile);
    if (!file.is_open())
        return;

    string q, a;
    while (getline(file, q) && getline(file, a))
    {
        memoryDB[q] = a;
    }
    file.close();
}

// Save memory to file
void saveMemory()
{
    ofstream file(memoryFile);
    for (auto &pair : memoryDB)
    {
        file << pair.first << endl;
        file << pair.second << endl;
    }
    file.close();
}

/*------------------------------
    SIMPLE EMOTION NETWORK
------------------------------*/

double act(double x)
{
    return 1.0 / (1.0 + exp(-x));
}

struct EmotionNet
{
    // input: question features
    // output: 4 emotions: curious, angry, happy, sad

    vector<double> predict(string text)
    {
        // Normalize text
        string t = text;
        transform(t.begin(), t.end(), t.begin(), ::tolower);

        double qMark = t.find("?") != string::npos ? 1 : 0;
        double exMark = t.find("!") != string::npos ? 1 : 0;
        double happy = (t.find("happy") != string::npos) ? 1 : 0;
        double sad = (t.find("sad") != string::npos || t.find("bad") != string::npos) ? 1 : 0;

        // Simple weighted network
        double curiousScore = act(qMark * 3 + 0.2);
        double angryScore = act(exMark * 3);
        double happyScore = act(happy * 4);
        double sadScore = act(sad * 4);

        return {curiousScore, angryScore, happyScore, sadScore};
    }

    string getEmotionName(vector<double> o)
    {
        double m = max(max(o[0], o[1]), max(o[2], o[3]));
        if (m == o[0])
            return "curious";
        if (m == o[1])
            return "angry";
        if (m == o[2])
            return "happy";
        return "sad";
    }
};

EmotionNet emo;

/*------------------------------
    PERSONALITY RESPONSES
------------------------------*/

string personalityRespond(string base, string emotion)
{
    if (emotion == "curious")
        return "🤔 " + base + ". Let me think about that.";
    if (emotion == "angry")
        return "😤 Okay okay... " + base + ". No need to shout!";
    if (emotion == "happy")
        return "😄 Yay! " + base + ". I'm happy to help!";
    if (emotion == "sad")
        return "😢 Oh… sorry you're feeling down. " + base;

    return base;
}
//topic detection
string detectTopic(string text)
{
    string t = text;
    transform(t.begin(), t.end(), t.begin(), ::tolower);

    if (t.find("hello") != string::npos || t.find("hi") != string::npos)
        return "greeting";

    if (t.find("thanks") != string::npos)
        return "thanks";

    if (t.find("sad") != string::npos || t.find("cry") != string::npos)
        return "sad";

    if (t.find("happy") != string::npos)
        return "happy";

    if (t.find("math") != string::npos)
        return "math";

    if (t.find("ai") != string::npos || t.find("machine") != string::npos)
        return "ai";

    if (t.find("love") != string::npos || t.find("like") != string::npos)
        return "love";

    if (t.find("game") != string::npos)
        return "game";

    return "unknown";
}
//create randomness

string randomResponse(string base)
{
    int r = rand() % 4;

    switch (r)
    {
    case 0:
        return base;
    case 1:
        return base + " 🙂";
    case 2:
        return "Well… " + base;
    case 3:
        return base + " 🤔";
    }
    return base;
}

//<<<<<<<<intigrate ai>>>>>>>>>
string hybridAI(string answer)
{
    // 1. Neural network emotion
    auto scores = emo.predict(answer);
    string emotion = emo.getEmotionName(scores);

    // 2. Topic detection
    string topic = detectTopic(answer);

    // 3. Base answer
    string base;

    if (topic == "greeting")
    {
        base = "Hello! How can I help?";
    }
    else if (topic == "thanks")
    {
        base = "You're welcome!";
    }
    else if (topic == "math")
    {
        base = "I love math! Ask me anything.";
    }
    else if (topic == "ai")
    {
        base = "AI is amazing! Want to learn more?";
    }
    else if (topic == "love")
    {
        base = "Love is complicated… but beautiful.";
    }
    else if (topic == "sad")
    {
        base = "I'm here for you. Want to talk?";
    }
    else if (topic == "happy")
    {
        base = "Yay! I’m happy for you!";
    }
    else
    {
        base = "I'm not sure. Can you explain more?";
    }

    // 4. Add personality flavor
    string flavored = personalityRespond(base, emotion);

    // 5. Add random humanization
    string final = randomResponse(flavored);

    return final;
}

// Find answer
string findAnswer(string question)
{
    if (memoryDB.find(question) != memoryDB.end())
        return memoryDB[question];
    return "";
}

int main()
{
    srand(time(NULL))
        loadMemory();

    cout << "🤖 Ai Memory Bot with Personality" << endl;

    string question;

    while (true)
    {
        cout << "You: ";
        getline(cin, question);
        question = lower(question);

        if (question == "exit")
        {
            break;
        }
        string answer = findAnswer(question);

        if (answer != "")
        {
            cout << "AI: " << hybridAI(answer) << "\n\n";
        }
        else
        {
            cout << "Ai: I don't know. \nAi: Teach me the answer \nyou: ";
            string newAns;
            getline(cin, newAns);

            memoryDB[question] = newAns;
            saveMemory();

            cout << "Ai: Thanks! I learned something new.\n\n";
        }
    }

    return 0;
}
