#include "py_console.h"

PyLog::PyLog() {}

PyLog::PyLog(PyStringType type_, char* content_) : type(type_), content(content_) {}

PyConsoleSingleton::PyConsoleSingleton() : interpreter(nullptr)
{
  ClearLog();
  memset(input_buf, 0, sizeof(input_buf));
  history_pos = -1;

  scroll_to_bottom = false;
}

PyConsoleSingleton::~PyConsoleSingleton()
{
  ClearLog();
  for (int i = 0; i < history.Size; i++)
    ImGui::MemFree(history[i]);
}

PyConsoleSingleton& PyConsoleSingleton::GetInstance()
{
  static PyConsoleSingleton _instance;
  return _instance;
}

void PyConsoleSingleton::LinkWith(PyInterpreterSingleton& interpreter_)
{
  interpreter = &interpreter_;
}

int   PyConsoleSingleton::Stricmp (const char* s1_, const char* s2_)         { int d; while ((d = toupper(*s2_) - toupper(*s1_)) == 0 && *s1_) { s1_++; s2_++; } return d; }
int   PyConsoleSingleton::Strnicmp(const char* s1_, const char* s2_, int n_) { int d = 0; while (n_ > 0 && (d = toupper(*s2_) - toupper(*s1_)) == 0 && *s1_) { s1_++; s2_++; n_--; } return d; }
char* PyConsoleSingleton::Strdup  (const char* s_)                           { IM_ASSERT(s_); size_t len = strlen(s_) + 1; void* buf = ImGui::MemAlloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s_, len); }
void  PyConsoleSingleton::Strtrim (char* s_)                                 { char* str_end = s_ + strlen(s_); while (str_end > s_ && str_end[-1] == ' ') str_end--; *str_end = 0; }

void PyConsoleSingleton::ClearLog()
{
  for (int i = 0; i < items.Size; i++)
    ImGui::MemFree(items[i].content);
  items.clear();
}

void PyConsoleSingleton::AddLog(PyStringType type_, const char* fmt_, ...)
{
  char _buf[1024];
  va_list _args;
  va_start(_args, fmt_);
  vsnprintf(_buf, IM_ARRAYSIZE(_buf), fmt_, _args);
  _buf[IM_ARRAYSIZE(_buf)-1] = 0;
  va_end(_args);
  items.push_back(PyLog(type_, Strdup(_buf)));
}

void PyConsoleSingleton::Draw(const char* title_, bool* p_open_)
{
  bool _is_copy_btn_clicked = false;

  ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
  if (!ImGui::Begin(title_, p_open_))
  {
    ImGui::End();
    return;
  }

  filter.Draw("Filter", 180);
  ImGui::Separator();

  const float _footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
  if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -_footer_height_to_reserve), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar))
  {
    if (ImGui::BeginPopupContextWindow())
    {
      _is_copy_btn_clicked = ImGui::Selectable("Copy");
      if (ImGui::Selectable("Clear")) ClearLog();
      ImGui::EndPopup();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
    if (_is_copy_btn_clicked)
      ImGui::LogToClipboard();
    for (auto item : items)
    {
      if (!filter.PassFilter(item.content))
        continue;
      ImVec4 _color;
      bool _has_color = false;
      switch(item.type)
      {
        case PyStringType::kNone:
          assert(false);
          break;
        case PyStringType::kNull:
        case PyStringType::kCommand:
          break;
        case PyStringType::kError:
          _has_color = true;
          _color = {1.0f, 0.0f, 0.0f, 1.0f};
          break;
        case PyStringType::kNormal:
          _has_color = true;
          _color = {0.0f, 0.3f, 0.3f, 1.0f};
          break;
      }
      if (_has_color) ImGui::PushStyleColor(ImGuiCol_Text, _color);
      ImGui::TextUnformatted(item.content);
      if (_has_color) ImGui::PopStyleColor();
    }
    if (_is_copy_btn_clicked)
      ImGui::LogFinish();

    if (scroll_to_bottom || (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
      ImGui::SetScrollHereY(1.0f);
    scroll_to_bottom = false;

    ImGui::PopStyleVar();
    ImGui::EndChild();
  }
  ImGui::Separator();

  bool _reclaim_focus = false;
  ImGuiInputTextFlags _input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
  if (ImGui::InputText("Input", input_buf, IM_ARRAYSIZE(input_buf), _input_text_flags, &TextEditCallbackStub, (void*)this))
  {
    char* s = input_buf;
    Strtrim(s);
    if (s[0])
      ExecCommand(s);
    strcpy(s, "");
    _reclaim_focus = true;
  }

  ImGui::SetItemDefaultFocus();
  if (_reclaim_focus)
    ImGui::SetKeyboardFocusHere(-1);

  ImGui::End();
}

void PyConsoleSingleton::ExecCommand(const char* command_line_)
{
  AddLog(PyStringType::kCommand, ">>> %s\n", command_line_);
  interpreter->SendCommand((const char8_t*)command_line_);

  // Insert into history. First find match and delete it so it can be pushed to the back.
  // This isn't trying to be smart or optimal.
  history_pos = -1;
  for (int i = history.Size - 1; i >= 0; i--)
  {
    if (Stricmp(history[i], command_line_) == 0)
    {
      ImGui::MemFree(history[i]);
      history.erase(history.begin() + i);
      break;
    }
  }
  history.push_back(Strdup(command_line_));
  
  PyResponse _response = interpreter->GetResponseBuf();
  if (_response.content.empty() == false)
    AddLog((PyStringType)_response.type, (const char*)_response.content.c_str());
  interpreter->ClearResponseBuf();

  // On command input, we scroll to bottom even if AutoScroll==false
  scroll_to_bottom = true;
}

// In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
int PyConsoleSingleton::TextEditCallbackStub(ImGuiInputTextCallbackData* data_)
{
  PyConsoleSingleton* _console = (PyConsoleSingleton*)data_->UserData;
  return _console->TextEditCallback(data_);
}

int PyConsoleSingleton::TextEditCallback(ImGuiInputTextCallbackData* data_)
{
  //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
  switch (data_->EventFlag)
  {
    case ImGuiInputTextFlags_CallbackHistory:
    {
      // Example of HISTORY
      const int _prev_history_pos = history_pos;
      if (data_->EventKey == ImGuiKey_UpArrow)
      {
        if (history_pos == -1)
          history_pos = history.Size - 1;
        else if (history_pos > 0)
          history_pos--;
      }
      else if (data_->EventKey == ImGuiKey_DownArrow)
      {
        if (history_pos != -1)
          if (++history_pos >= history.Size)
            history_pos = -1;
      }

      // A better implementation would preserve the data on the current input line along with cursor position.
      if (_prev_history_pos != history_pos)
      {
        const char* history_str = (history_pos >= 0) ? history[history_pos] : "";
        data_->DeleteChars(0, data_->BufTextLen);
        data_->InsertChars(0, history_str);
      }
    }
  }

  return 0;
};

void PyConsoleSingleton::ShowPythonConsole(bool* p_open_)
{
  PyConsoleSingleton& _console = PyConsoleSingleton::GetInstance();
  assert(_console.interpreter != nullptr);
  _console.Draw("Python Console", p_open_);
};