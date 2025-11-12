#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <ctime>
#include <limits>
#include <stdexcept>
#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <stack>

using namespace std;

//Mood entry data
struct MoodEntry {
    string date;
    string mood;
    string note;
    int score;
    MoodEntry* next; // linked list pointer
    MoodEntry(string d, string m, string n, int s) : date(d), mood(m), note(n), score(s), next(nullptr) {}
};

//Exceptions
class FileException : public runtime_error {
public:
    FileException(const string &msg) : runtime_error(msg) {}
};

class InputException : public invalid_argument {
public:
    InputException(const string &msg) : invalid_argument(msg) {}
};

//Base class for analysis
class MoodAnalyzer {
public:
    virtual pair<string,int> analyze(const string &input) = 0; // pure virtual
    virtual ~MoodAnalyzer() = default;
};

//Derived class for mood analyzer
class AIAnalyzer : public MoodAnalyzer {
private:
    // List of keywords
    static constexpr int N_POS = 20;
    static constexpr int N_NEG = 21;
    string posWords[N_POS] = {
        "happy", "good", "great", "excited", "joy", "relieved",
        "calm", "peaceful", "motivated", "hopeful", "grateful", "energetic",
        "cheerful", "content", "satisfied", "amazing", "wonderful", "optimistic",
        "confident", "fantastic"
    };
    int posWeights[N_POS] = { 3, 2, 2, 2, 3, 2,
    2, 3, 2, 2, 3, 2,
    2, 2, 2, 3, 3, 2,
    3, 3};

    string negWords[N_NEG] ={"sad", "tired", "angry", "stressed", "anxious", "depressed",
    "lonely", "upset", "exhausted", "worried", "frustrated", "bored",
    "nervous", "hopeless", "disappointed", "irritated", "hurt", "lost",
    "drained", "guilty", "bad"};
    int negWeights[N_NEG] = {-3, -1, -2, -2, -2, -3,
    -2, -1, -2, -2, -2, -1,
    -2, -3, -2, -2, -2, -3,
    -2, -2, -2};

    // neutral set
    vector<string> neutralWords = {"ok", "fine", "soso", "meh", "okay",
    "normal", "average", "alright", "nothing much", "decent"};

    // turn string to lowercase
    string toLower(string s) {
        for (char &c : s) c = tolower(c);
        return s;
    }

    // recursively tokenize and score words
    int recursiveScore(const vector<string> &tokens, int idx) {
        if (idx < 0) return 0;
        int score = 0;
        string t = tokens[idx];
        // check positive list
        for (int i = 0; i < N_POS; ++i) if (t.find(posWords[i]) != string::npos) score += posWeights[i];
        // check negative list
        for (int i = 0; i < N_NEG; ++i) if (t.find(negWords[i]) != string::npos) score += negWeights[i];
        // neutrals have no effect for now (kept for future use)
        return score + recursiveScore(tokens, idx - 1);
    }

public:
    virtual pair<string,int> analyze(const string &input) override {
        if (input.empty()) throw InputException("Empty input for analysis.");
        string s = toLower(input);
        // splitting sentence to individual words
        vector<string> tokens;
        string token;
        for (char c : s) {
            if (isalnum(static_cast<unsigned char>(c))) token.push_back(c);
            else {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            }
        }
        if (!token.empty()) tokens.push_back(token);

        int totalScore = recursiveScore(tokens,(int)tokens.size()-1); // recursion used here

        string mood;
        if (totalScore >= 3) mood = "Happy";
        else if (totalScore > 0) mood = "Positive";
        else if (totalScore == 0) mood = "Neutral";
        else if (totalScore > -4) mood = "Negative";
        else mood = "Depressed";

        return {mood, totalScore};
    }
};

// Quote recommendation system
class Recommendation {
private:
    vector<string> positiveTips;
    vector<string> neutralTips;
    vector<string> negativeTips;
    vector<string> depressedTips;

public:
    Recommendation() {
        // Default positive tips
        positiveTips = {
            "Keep that energy up, share your happiness with others!",
            "Celebrate your wins, no matter how small.",
            "Use this motivation to work on a goal you've been postponing.",
            "Stay grateful, positivity is contagious!",
            "Keep smiling, your good energy can brighten someone's day!"
        };

        // Default neutral tips
        neutralTips = {
            "Take a few minutes to reflect on how you feel.",
            "Do something simple that makes you smile today.",
            "Sometimes doing nothing is okay, give yourself a moment.",
            "Listen to some music that fits your current mood.",
            "Go for a short walk to clear your head."
        };

        // Default negative tips
        negativeTips = {
            "It is okay to not be okay, be gentle with yourself.",
            "Try taking a break and focusing on your breathing.",
            "Talk to someone you trust about whats on your mind.",
            "Write down what's bothering you, releasing helps.",
            "Small steps count. Focus on one positive thing today."
        };

        // Default depressed tips
        depressedTips = {
            "You are not alone, reaching out for help is a brave step.",
            "Please take things one day at a time, you've made it this far.",
            "Talk to a trusted friend or counselor. You matter.",
            "Rest and take care of yourself, recovery starts small.",
            "It is okay to ask for support. You deserve kindness and care."
        };

        // load extra user-added tips from files
        loadTipsFromFile("positive_tips.txt", positiveTips);
        loadTipsFromFile("neutral_tips.txt", neutralTips);
        loadTipsFromFile("negative_tips.txt", negativeTips);
        loadTipsFromFile("depressed_tips.txt", depressedTips);
    }

    // Function to load tips from file
    void loadTipsFromFile(const string &filename, vector<string> &category) {
        ifstream ifs(filename);
        if (ifs) {
            string line;
            while (getline(ifs, line)) {
                if (!line.empty())
                    category.push_back(line);
            }
            ifs.close();
        }
    }

    // Function to get random tip based on mood
    string getTip(const string &mood) {
        srand(time(0));
        vector<string> selected;

        if (mood == "Happy" || mood == "Positive")
            selected = positiveTips;
        else if (mood == "Neutral")
            selected = neutralTips;
        else if (mood == "Negative")
            selected = negativeTips;
        else if (mood == "Depressed")
            selected = depressedTips;
        else
            selected = neutralTips;

        if (selected.empty()) return "Take care and stay mindful.";
        int index = rand() % selected.size();
        return selected[index];
    }

    // Allow user to add their own tip
    void addUserTip(const string &mood, const string &tip) {
        string filename;

        if (mood == "Happy" || mood == "Positive") filename = "positive_tips.txt";
        else if (mood == "Neutral") filename = "neutral_tips.txt";
        else if (mood == "Negative") filename = "negative_tips.txt";
        else if (mood == "Depressed") filename = "depressed_tips.txt";
        else filename = "neutral_tips.txt";

        ofstream ofs(filename, ios::app);
        if (ofs) {
            ofs << tip << endl;
            ofs.close();
        }

        cout << "Your new tip has been saved to " << filename << "!" << endl;
    }
};


// reading/writing mood history
class FileManager {
public:
    // write mood history to file
    static void saveHistory(const string &filename, MoodEntry* head) {
        ofstream ofs(filename);
        if (!ofs) throw FileException("Cannot open file for saving: " + filename);
        MoodEntry* cur = head;
        while (cur) {
            // Basic escaping: replace newlines by space
            string note = cur->note;
            for (char &c : note) if (c == '\n') c = ' ';
            ofs << cur->date << '\t' << cur->mood << '\t' << cur->score << '\t' << note << '\n';
            cur = cur->next;
        }
        ofs.close();
    }

    // load mood history from file
    static MoodEntry* loadHistory(const string &filename) {
        ifstream ifs(filename);
        if (!ifs) throw FileException("Cannot open file for loading: " + filename);
        MoodEntry* head = nullptr;
        MoodEntry* tail = nullptr;
        string line;
        while (getline(ifs, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string date, mood, scoreStr, note;
            if (!getline(ss, date, '\t')) continue;
            if (!getline(ss, mood, '\t')) continue;
            if (!getline(ss, scoreStr, '\t')) continue;
            if (!getline(ss, note)) note = "";
            // guard against bad score format
            try {
                int score = stoi(scoreStr);
                MoodEntry* node = new MoodEntry(date, mood, note, score);
                if (!head) head = tail = node;
                else { tail->next = node; tail = node; }
            } catch (...) {
                // malformed score: skip line
                continue;
            }
        }
        ifs.close();
        return head;
    }
};

// linked list for mood history
class MoodHistory {
private:
    MoodEntry* head;
public:
    MoodHistory() : head(nullptr) {}

    ~MoodHistory() {
        clear();
    }

    void addEntry(const string &date, const string &mood, const string &note, int score) {
        MoodEntry* node = new MoodEntry(date, mood, note, score);
        // insert at end
        if (!head) head = node;
        else {
            MoodEntry* cur = head;
            while (cur->next) cur = cur->next;
            cur->next = node;
        }
    }

    // recursive printing of linked list
    void printRecursive(MoodEntry* node) {
        if (!node) return;
        cout << node->date << " | " << node->mood << " | score=" << node->score << " | note: " << node->note << '\n';
        printRecursive(node->next);
    }

    void showAll() {
        if (!head) { cout << "No entries yet.\n"; return; }
        cout << "---- Mood History ----\n";
        printRecursive(head);
        cout << "----------------------\n";
    }

    MoodEntry* getHead() { return head; }

    // undo last entry: remove last node (using pointers)
    MoodEntry* removeLast() {
        if (!head) return nullptr;
        MoodEntry* cur = head;
        MoodEntry* prev = nullptr;
        while (cur->next) {
            prev = cur;
            cur = cur->next;
        }
        if (!prev) {
            head = nullptr;
        } else {
            prev->next = nullptr;
        }
        return cur; // caller owns pointer
    }

    // convert to vector of moods for analytics
    vector<int> getScores() {
        vector<int> res;
        MoodEntry* cur = head;
        while (cur) {
            res.push_back(cur->score);
            cur = cur->next;
        }
        return res;
    }

    void clear() {
        MoodEntry* cur = head;
        while (cur) {
            MoodEntry* tmp = cur->next;
            delete cur;
            cur = tmp;
        }
        head = nullptr;
    }
};


// User login and registration
class UserException : public runtime_error {
public:
    explicit UserException(const string &msg) : runtime_error(msg) {}
};

class UserManager {
private:
    string userFile = "users.db";
    string currentUser;

public:
    // Register New User
    bool registerUser(const string &username, const string &password) {
        if (username.empty() || password.empty())
            throw UserException("Username or password cannot be empty!");

        ifstream ifs(userFile);
        string line;
        while (getline(ifs, line)) {
            if (line.empty()) continue;
            size_t pos = line.find(':');
            if (pos == string::npos) continue;
            string u = line.substr(0,pos);
            if (u == username) {
                throw UserException("Username already exists. Please choose another one.");
            }
        }
        ifs.close();

        ofstream ofs(userFile, ios::app);
        if (!ofs)
            throw UserException("Unable to open user database for writing!");
        ofs << username << ':' << password << '\n';
        ofs.close();

        currentUser = username; // Set the current user after successful registration

        cout << "Account created successfully for " << username << "!\n";
        return true;
    }

    // Login Existing User
    bool login(const string &username, const string &password) {
        if (username.empty() || password.empty())
            throw UserException("Username or password cannot be empty!");

        ifstream ifs(userFile);
        if (!ifs)
            throw UserException("No user database found. Please register first.");

        string line;
        while (getline(ifs, line)) {
            if (line.empty()) continue;
            size_t pos = line.find(':');
            if (pos == string::npos) continue;
            string u = line.substr(0,pos);
            string p = line.substr(pos+1);
            // remove possible carriage return at end (windows files)
            if (!p.empty() && p.back() == '\r') p.pop_back();
            if (u == username && p == password) {
                currentUser = username;
                return true;
            }
        }
        throw UserException("Incorrect username or password.");
    }

    string getCurrentUser() const { return currentUser; }
};

//  template to calculate average mood score
template<typename T>
double average(const vector<T> &arr) {
    if (arr.empty()) return 0.0;
    double sum = 0;
    for (auto &x : arr) sum += static_cast<double>(x);
    return sum / arr.size();
}

// undo mood entry
class UndoStack {
private:
    stack<MoodEntry*> stk;
public:
    void push(MoodEntry* node) { stk.push(node); }
    MoodEntry* pop() {
        if (stk.empty()) return nullptr;
        MoodEntry* t = stk.top(); stk.pop();
        return t;
    }
    bool empty() const { return stk.empty(); }
};

// predict next mood
string predictNextMood(MoodHistory &history, int lookback = 5) {
    // look at last N entries, choose most frequent mood
    vector<string> moods;
    MoodEntry* cur = history.getHead();
    // move to end and collect last N by using vector
    while (cur) { moods.push_back(cur->mood); cur = cur->next; }
    if (moods.empty()) return "No data";
    unordered_map<string,int> freq;
    int count = 0;
    for (int i = (int)moods.size()-1; i >=0 && count < lookback; --i, ++count) {
        freq[moods[i]]++;
    }
    // find max
    string best = "";
    int bestc = -1;
    for (auto &p : freq) {
        if (p.second > bestc) { best = p.first; bestc = p.second; }
    }
    return best;
}

// Record date
string todayDate() {
    time_t t = time(nullptr);
    tm *tm = localtime(&t);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", tm);
    return string(buf);
}


// Menu
void showMenu() {
    cout << "\n=== MoodMate AI+ ===\n";
    cout << "1. Enter today's mood\n";
    cout << "2. Show mood history\n";
    cout << "3. Undo last entry\n";
    cout << "4. Show analytics\n";
    cout << "5. Export history to file\n";
    cout << "6. Import history from file\n";
    cout << "7. Add custom quote\n";
    cout << "8. Predict next mood\n";
    cout << "9. Restore last undo\n";
    cout << "10. Exit\n";
    cout << "Choose option: ";
}

int main() {
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    cout << "\n---Welcome to MoodMate AI+---\n";
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;

    UserManager um;
    string username, password;
    bool loggedIn = false; // Add a flag to track login status

    // login and register menu
    int loginChoice;
    cout << "\n=== ACCOUNT MENU ===\n";
    cout << "1. Login\n";
    cout << "2. Register\n";
    cout << "Choose option: ";
    cin >> loginChoice;
    cin.ignore();

    try {
        if (loginChoice == 1) {
            cout << "Enter username: ";
            getline(cin, username);
            cout << "Enter password: ";
            getline(cin, password);

            if (um.login(username, password)) {
                cout << "Login successful. Welcome, " << um.getCurrentUser() << "!\n";
                loggedIn = true; // Set flag
                }
        }
        else if (loginChoice == 2) {
            cout << "Enter new username: ";
            getline(cin, username);
            cout << "Enter new password: ";
            getline(cin, password);

            if (um.registerUser(username, password)) {
                cout << "You are now logged in as " << um.getCurrentUser() << ".\n";
                loggedIn = true; // Set flag
            }
            }
        else {
            cout << "Invalid option. Exiting.\n";
            return 0;
            }
        }
        catch (UserException &e) {
            cout << "Error: " << e.what() << "\n";
            return 0;
        }

        // if registration failed somehow)
        if (!loggedIn) {
            return 0;
        }

        // retrieve the username after successful login or registration
        username = um.getCurrentUser();

    // Continue program only if login successful
    string filename = username + "_history.txt";
    MoodHistory history;
    Recommendation rec;
    AIAnalyzer analyzer;
    UndoStack undoStack;

    // Try loading user's past data
    try {
        MoodEntry* loaded = FileManager::loadHistory(filename);
        if (loaded) {
            MoodEntry* cur = loaded;
            while (cur) {
                history.addEntry(cur->date, cur->mood, cur->note, cur->score);
                cur = cur->next;
            }
            // cleanup loaded
            MoodEntry* tmp = loaded;
            while (tmp) { MoodEntry* t2 = tmp->next; delete tmp; tmp = t2; }

            cout << "Previous history loaded for user: " << username << "\n";
        }
    }
    catch (FileException &e) {
        cout << "(No previous history found for this user.)\n";
    }

    while (true) {
        showMenu();
        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice. Try again.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        try {
            if (choice == 1) {
                cout << "How are you feeling today? (write a short sentence):\n> ";
                string input;
                getline(cin, input);
                auto result = analyzer.analyze(input);
                string mood = result.first;
                int score = result.second;
                cout << "Detected mood: " << mood << " (score=" << score << ")\n";

                // use mood-aware recommendation
                string suggestion = rec.getTip(mood);
                cout << "Suggestion: " << suggestion << "\n";

                cout << "Save entry? (y/n): ";
                char c; cin >> c;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (c=='y' || c=='Y') {
                    string date = todayDate();
                    history.addEntry(date, mood, input, score);
                    cout << "Saved.\n";
                } else {
                    cout << "Not saved.\n";
                }
            }else if (choice == 2) {
                history.showAll();
            }else if (choice == 3) {
                MoodEntry* removed = history.removeLast();
                if (!removed) {
                    cout << "No entries to undo.\n";
                }else {
                    // push to undo stack so user can restore if needed
                    undoStack.push(removed);
                    cout << "Last entry removed (you can restore using option 9).\n";
                }
            }else if (choice == 4) {
                // analytics
                vector<int> scores = history.getScores();
                double avg = average(scores);
                cout << fixed << setprecision(2);
                cout << "Average mood score: " << avg << '\n';
                // count moods
                unordered_map<string,int> freq;
                MoodEntry* cur = history.getHead();
                while (cur) { freq[cur->mood]++; cur = cur->next; }
                cout << "Mood counts:\n";
                for (auto &p : freq) {
                    cout << " - " << p.first << " : " << p.second << " ";
                    for (int i = 0; i < p.second; ++i) 
                    cout << '\n';
                }
            } else if (choice == 5) {
                cout << "Export filename (default: " << filename << "): ";
                string fn; getline(cin, fn);
                if (fn.empty()) fn = filename;
                FileManager::saveHistory(fn, history.getHead());
                cout << "Exported to " << fn << "\n";
            } else if (choice == 6) {
                cout << "Import filename: ";
                string fn; getline(cin, fn);
                if (fn.empty()) { cout << "No filename given.\n"; continue; }
                MoodEntry* loaded = FileManager::loadHistory(fn);
                if (!loaded) { cout << "No entries found in file.\n"; continue; }
                MoodEntry* cur = loaded;
                while (cur) {
                    history.addEntry(cur->date, cur->mood, cur->note, cur->score);
                    cur = cur->next;
                }
                // cleanup loaded
                MoodEntry* tmp = loaded;
                while (tmp) { MoodEntry* t2 = tmp->next; delete tmp; tmp = t2; }
                cout << "Imported from " << fn << "\n";
            } else if (choice == 7) {
                cout << "Enter the mood category for your tip (Happy / Neutral / Negative / Depressed): ";
                string mood;
                getline(cin, mood);

                cout << "Enter your custom tip or quote:\n> ";
                string tip;
                getline(cin, tip);

                rec.addUserTip(mood, tip);
                cout << "Your tip has been added and saved!\n";
            }else if (choice == 8) {
                string prediction = predictNextMood(history, 5);
                cout << "Predicted next mood (based on last 5 entries): " << prediction << '\n';
            } else if (choice == 9) {
                // Restore last undone (if any)
                MoodEntry* toRestore = undoStack.pop();
                if (!toRestore) {
                    cout << "Nothing to restore.\n";
                } else {
                    history.addEntry(toRestore->date, toRestore->mood, toRestore->note, toRestore->score);
                    delete toRestore; // avoid leak (we've copied the data)
                    cout << "Last undone entry has been restored.\n";
                }
            } else if (choice == 10) {

                // save current history automatically
                try {
                    FileManager::saveHistory(filename, history.getHead());
                    cout << "Saved history to " << filename << '\n';
                } catch (FileException &e) {
                    cout << "Warning: could not save history: " << e.what() << '\n';
                }
                // free any nodes left on undo stack
                while (!undoStack.empty()) {
                    MoodEntry* rem = undoStack.pop();
                    delete rem;
                }
                cout << "Goodbye!\n";
                break;
            } else {
                cout << "Unknown option.\n";
            }
        } catch (FileException &fe) {
            cout << "File error: " << fe.what() << '\n';
        } catch (InputException &ie) {
            cout << "Input error: " << ie.what() << '\n';
        } catch (exception &e) {
            cout << "General error: " << e.what() << '\n';
        }
    }
    return 0;
}
