:: ============================================================================
:: MyShell — Automated Acceptance Test Suite
:: Author: Role 4 (System Utilities & Acceptance Testing)
:: Usage inside Small Shell: run Testcase\test_basic.bat
:: ============================================================================


time
date
path


set_env OS_PROJECT_SCORE 10_Points
print_env OS_PROJECT_SCORE
unset_env OS_PROJECT_SCORE
print_env OS_PROJECT_SCORE


pwd
mkdir test_workspace
cd test_workspace
pwd
create_file demo.txt
write_file Hello_Operating_System_Class demo.txt
read_file demo.txt
copy_file demo.txt backup.txt
dir
delete_file demo.txt
delete_file backup.txt
cd ..
rmdir test_workspace


list
list_processes


history
help run