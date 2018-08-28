#include "board.hpp"

class CmdProcess {
public:
	CmdProcess(Board* board);

	virtual bool process(vector<string> args) = 0;

	const string& get_msg() const;
	const string& get_name() const;

	//static void set_board(Board* board);

protected:
	Board* m_board;

	string m_msg;

	string m_name;

	//string m_disc;
};

typedef map<const string, CmdProcess*> CmdProcMap;

class CmdModule : public CmdProcess {
public:
	CmdModule(Board* board);
	virtual bool process(vector<string> args);
};

class CmdSet : public CmdProcess {
public:
	CmdSet(Board* board) : CmdProcess(board) {};
	virtual bool process(vector<string> args);
};

class CmdGet : public CmdProcess {
public:
	CmdGet(Board* board) : CmdProcess(board) {};
	virtual bool process(vector<string> args);
};

class CmdLsMod : public CmdProcess {
public:
	CmdLsMod(Board* board) : CmdProcess(board) {};
	virtual bool process(vector<string> args);
};

class CmdLsPort : public CmdProcess {
public:
	CmdLsPort(Board* board) : CmdProcess(board) {};
	virtual bool process(vector<string> args);
};

class CmdRun : public CmdProcess {
public:
	CmdRun(Board* board) : CmdProcess(board) {};
	virtual bool process(vector<string> args);
};

class CmdFinish : public CmdProcess {
public:
	CmdFinish(Board* board) : CmdProcess(board) {};
	virtual bool process(vector<string> args);
};

class CmdPing : public CmdProcess {
public:
	CmdPing(Board* board) : CmdProcess(board) {};
	virtual bool process(vector<string> args);
};
