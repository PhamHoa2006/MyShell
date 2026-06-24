:: ==========================================
:: CASE STUDY 1: Workspace Setup
:: ==========================================
:: 1. Create a new directory for the presentation
mkdir Presentation

:: 2. Move into the new directory
cd Presentation

:: 3. Create a configuration file
mkfile config.txt

:: 4. Write some initial data to the file
write "Project-initialized-successfully!" config.txt

:: 5. Read the file to verify its contents
cat config.txt

:: 6. Move back to the parent directory
cd ..

:: 7. Back up the entire Presentation folder
cpdir Presentation Backup_Presentation

:: 8. Display the directory tree to show the new folders
tree

:: ==========================================
:: CLEANUP: Remove folders so the script can be run again safely
:: ==========================================
