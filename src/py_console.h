#include "py_interpreter.h"
#include "imgui.h"

typedef struct PyLog
{
  PyStringType type;
  char* content;

  PyLog();
  PyLog(PyStringType type_, char* content_);
  ~PyLog() = default;
} PyLog;

struct PyConsoleSingleton
{
  PyInterpreterSingleton* interpreter;

  char                  input_buf[256];
  ImVector<PyLog>       items;
  //ImVector<const char*> commands;
  ImVector<char*>       history;
  int                   history_pos;
  ImGuiTextFilter       filter;
  //bool                  auto_scroll;
  bool                  scroll_to_bottom;

  PyConsoleSingleton();
  ~PyConsoleSingleton();
  static PyConsoleSingleton& GetInstance();
  void LinkWith(PyInterpreterSingleton& interpreter_);
  static int   Stricmp (const char* s1_, const char* s2_);
  static int   Strnicmp(const char* s1_, const char* s2_, int n_);
  static char* Strdup  (const char* s_);
  static void  Strtrim (char* s_);
  void ClearLog();
  void AddLog(PyStringType type_,const char* fmt_, ...) IM_FMTARGS(3);
  void Draw(const char* title_, bool* p_open_);
  void ExecCommand(const char* command_line_);
  static int TextEditCallbackStub(ImGuiInputTextCallbackData* data_);
  int TextEditCallback(ImGuiInputTextCallbackData* data_);
  static void ShowPythonConsole(bool* p_open_);
};