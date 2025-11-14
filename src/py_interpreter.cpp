#include "py_interpreter.h"

PyResponse::PyResponse() : type(PyStringType::kNone), content(u8"") {}

PyResponse::PyResponse(PyStringType type_, const std::u8string& content_) : type(type_), content(content_) {}

PyInterpreterSingleton::PyInterpreterSingleton() : response_buf() {}

PyInterpreterSingleton& PyInterpreterSingleton::GetInstance()
{
  static PyInterpreterSingleton _instance;
  return _instance;
}

void PyInterpreterSingleton::SetProperties(const char* main_argv_0_, const char* err_msg_ )
{
  fs::path _exe_file_path = fs::canonical(main_argv_0_);
  fs::path _exe_file_dir_path = _exe_file_path.parent_path();
  fs::path _py_root_path = _exe_file_dir_path / "python";
  const wchar_t* _exe_file_path_wchar = Py_DecodeLocale(_exe_file_path.string().c_str(), nullptr);
  const wchar_t* _py_root_path_wchar  = Py_DecodeLocale( _py_root_path.string().c_str(), nullptr);
  if (_exe_file_path_wchar == nullptr || _py_root_path_wchar == nullptr)
  {
    if (err_msg_ != nullptr)
      err_msg_ = "FATAL: Cannot decode argv[0]";
    return;
  }

  PyStatus _status;
  PyConfig _config;
  PyConfig_InitIsolatedConfig(&_config);

  _status = PyConfig_SetString(&_config, &_config.program_name, _exe_file_path_wchar);
  if (ThereIsPyConfigErr(_status, err_msg_) == true) return;

  _status = PyConfig_SetString(&_config, &_config.home, _py_root_path_wchar);
  if (ThereIsPyConfigErr(_status, err_msg_) == true) return;

  _status = PyConfig_Read(&_config);
  if (ThereIsPyConfigErr(_status, err_msg_) == true) return;

  _config.module_search_paths_set = 1;
  std::array<fs::path, 4> path_arr = 
  {
      fs::path(_exe_file_dir_path / "python" / "DLLs"),
      fs::path(_exe_file_dir_path / "python" / "Lib"),
      fs::path(_exe_file_dir_path / "python" / "Lib" / "site-packages"),
      fs::path(_exe_file_dir_path / "python-scripts")
  };
  for (auto iter : path_arr)
  {
    _status = PyWideStringList_Append(&_config.module_search_paths, iter.wstring().c_str());
    if (ThereIsPyConfigErr(_status, err_msg_) == true) return;
  }

  _status = Py_InitializeFromConfig(&_config);
  if (ThereIsPyConfigErr(_status, err_msg_) == true) return;

  PyConfig_Clear(&_config);

  py::module_ scope = py::module::import("__main__");
  scope.def("write_py_interp_response_at_log", &PyInterpreterSingleton::WritePyInterpResponseLog);

  try
  {
    py::exec(
R"(
import sys
class SimpleRedirector:
  def __init__(self, write_func):
    self.write_func = write_func
  def write(self, data):
    if data.strip():
      self.write_func(data)
  def flush(self):
    pass
sys.stdout = SimpleRedirector(write_py_interp_response_at_log)
sys.stderr = SimpleRedirector(write_py_interp_response_at_log)
)"
    );
  } 
  catch (const py::error_already_set& e) 
  {
    return;
  }

  py_root_path = _py_root_path;
  return;
}

void PyInterpreterSingleton::StockUpResponse(PyStringType type_, std::u8string content_)
{
  std::lock_guard<std::mutex> lock(response_buf_mutex);
  response_buf = PyResponse(type_, content_.append(u8"\n"));
}

PyResponse& PyInterpreterSingleton::GetResponseBuf()
{
  return response_buf;
}

void PyInterpreterSingleton::ClearResponseBuf()
{
  response_buf.type = PyStringType::kNone;
  response_buf.content.clear();
}

// *** README ***
// Daesung Coding Convension doesn't recommend error try-catch.
// It is only allowed for wrapping function when 3rd party library should be used,
// however, it should be discussed with a team necessary of the such 3rd party library before using it.
void PyInterpreterSingleton::SendCommand(const std::u8string& content_)
{
  try 
  {
    py::exec((const char*)content_.c_str());
    if (response_buf.content.empty())
    {
      response_buf.type = PyStringType::kNull;
      response_buf.content.clear();
    }
  } 
  catch (const py::error_already_set& e) 
  {
    StockUpResponse(PyStringType::kError, std::u8string((char8_t*)e.what()));
  }
}

void PyInterpreterSingleton::WritePyInterpResponseLog(const std::u8string& content_)
{
  PyInterpreterSingleton& _instance = PyInterpreterSingleton::GetInstance();
  _instance.StockUpResponse(PyStringType::kNormal, content_);
}

bool PyInterpreterSingleton::ThereIsPyConfigErr(PyStatus& status_, const char* err_msg_)
{
  if (status_._type != PyStatus::_PyStatus_TYPE_OK)
  {
    if (err_msg_ != nullptr)
      err_msg_ = status_.err_msg;
    return true;
  }
  return false;
}


// ***** Test Code *****

// #include <fstream>

// int main(int, const char** argv)
// {
//   // Initialize python interpreter
//   PyInterpreterSingleton& _instance = PyInterpreterSingleton::GetInstance();
//   const char* _err_msg = nullptr;
//   _instance.SetProperties(argv_[0], _err_msg);
//   if (_err_msg != nullptr)
//   {
//     printf("FATAL ERROR : %s", _err_msg);
//     return 1;
//   }

//   std::ofstream out("test.txt");

//   _instance.SendCommand(u8"import os");
//   _instance.SendCommand(u8"print(os.walk)");
//   _instance.SendCommand(u8"dsadfdasad");
//   _instance.SendCommand(u8"print(\"테스트 구문\")");
//   _instance.SendCommand(u8"12345abcde");
//   _instance.SendCommand(u8"print(\"이것도 테스트 문장입니다.\")");

//   for (auto iter = _instance.GetRecord().begin(); iter < _instance.GetRecord().end(); iter++)
//   {
//     out << (char*)((*iter).content.c_str());
//   }

//   return 0;
// }

// *********************