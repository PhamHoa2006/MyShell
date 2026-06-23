:: ==========================================
:: CASE STUDY 2: System Environment
:: ==========================================
:: 1. Check the current system time
time

:: 2. Check the current system date
date

:: 3. List all current local environment variables (should be empty initially)
list_env

:: 4. Set a new environment variable for our project
set_env PROJECT_MODE Presentation_Demo

:: 5. Verify the variable was set correctly
print_env PROJECT_MODE
