namespace app {
namespace dialogs {

class Options {
 public:


  Options();
  ~Options();
  void Show(core::windows::Window* parent);
  void OnInit(HWND hwndDlg);
 protected:
  bool init_;  
  core::windows::Window* parent_;
  HWND handle_;
  INT_PTR create_result;
  void set_cpu_freq_text(double cpu_freq);
  void copy_to_volatile();
  void copy_from_volatile();
  static INT_PTR CALLBACK DialogProc(__in  HWND hwndDlg,__in  UINT uMsg,__in  WPARAM wParam,__in  LPARAM lParam);
};

}
}