#ifndef OpenKAI_src_Script_Kiss_H_
#define OpenKAI_src_Script_Kiss_H_

#include "../Base/common.h"
#include "../Base/BASE.h"
#include "JSON.h"

using namespace std;

namespace kai
{

	class Kiss
	{
	public:
		Kiss(void);
		~Kiss(void);

		bool parse(const string& s);
		Kiss *child(const string &name);
		Kiss *child(int i);
		Kiss *root(void);
		Kiss *parent(void);
		bool empty(void);

		Kiss *find(const string &name);
		void *getInst(const string &name);

		template <typename T>
		bool v(const string &name, T *pVal)
		{
			return m_json.v(name, pVal);
		}

		template <typename T>
		int a(const string &name, vector<T> *pVal)
		{
			return m_json.a(name, pVal);
		}


		JSON *json(void);

	private:
		void trim(string *pS);
		void delComment(string *pS);
		bool addChild(const string& s);

	public:
		string m_class;
		string m_name;
		JSON m_json;
		BASE *m_pInst;

		Kiss *m_pParent;
		vector<Kiss *> m_vChild;

		Kiss *m_pNULL;
		bool m_bNULL;
	};

}
#endif
