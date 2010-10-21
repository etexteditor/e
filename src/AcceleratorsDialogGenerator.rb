name = "output"

file = File.new('AcceleratorsDialog.html')
size = file.read.size

file = File.new('AcceleratorsDialog.html')
str = file.read.gsub(/([\\\"])/) {|match| "\\#{match}"}
str = str.split("\n").map do |line|
  "#{name}->Append(wxT(\"#{line}\\n\"));"
end.join("\n")

output = "#include \"wx/wx.h\"
wxString* AcceleratorsDialogHtml() {
  wxString* #{name} = new wxString();
  #{name}->Alloc(#{size});
  #{str}
  return #{name};
}"

File.open('AcceleratorsDialogHtml.h', 'w+') {|file| file.write output }