#include "py_interpreter.h"
#include "imgui.h"

struct PyConsoleSingleton
{
  PyInterpreterSingleton* interpreter;

  char                  input_buf[256];
  ImVector<char*>       items;
  //ImVector<const char*> commands;
  ImVector<char*>       history;
  int                   history_pos;
  ImGuiTextFilter       filter;
  //bool                  auto_scroll;
  bool                  scroll_to_bottom;

  PyConsoleSingleton();
  ~PyConsoleSingleton();
  static PyConsoleSingleton& GetInstance();
  void LinkWithInterpreter(PyInterpreterSingleton& interpreter_);
  static int   Stricmp (const char* s1_, const char* s2_);
  static int   Strnicmp(const char* s1_, const char* s2_, int n_);
  static char* Strdup  (const char* s_);
  static void  Strtrim (char* s_);
  void ClearLog();
  void AddLog(const char* fmt_, ...) IM_FMTARGS(2);
  void Draw(const char* title_, bool* p_open_);
  void ExecCommand(const char* command_line_);
  static int TextEditCallbackStub(ImGuiInputTextCallbackData* data_);
  int TextEditCallback(ImGuiInputTextCallbackData* data_);
  static void ShowPythonConsole(bool* p_open_);
};