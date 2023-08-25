#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <windows.h>
namespace fs = std::filesystem;

std::string ReplaceAll(std::string str, const std::string &from, const std::string &to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

void displayHelp()
{
    std::cout << "Available commands:" << std::endl;
    std::cout << "  help               - Display this help message." << std::endl;
    std::cout << "  create [file]      - Create a new empty file." << std::endl;
    std::cout << "  delete [file]      - Delete a specified file." << std::endl;
    std::cout << "  rename [old] [new] - Rename a file from old name to new name." << std::endl;
    std::cout << "  move [file] [dir]  - Move a specified file to a target directory." << std::endl;
    std::cout << "  copy [file] [dir]  - Copy a specified file to a target directory." << std::endl;
    std::cout << "  mkdir [dir]        - Create a new directory." << std::endl;
    std::cout << "  rmdir [dir]        - Remove a directory (and its contents if specified)." << std::endl;
    std::cout << "  ls                 - List files and directories in the current directory." << std::endl;
    std::cout << "  cd [dir]           - Change the current directory.\nType the command followed by the required parameters." << std::endl;
}

void createFile(const std::string &fileName)
{
    std::ofstream file(fileName);
    if (file.is_open())
    {
        std::cout << "File created successfully." << std::endl;
        file.close();
    }
    else
    {
        std::cout << "Error creating file." << std::endl;
    }
}

void deleteFile(const std::string &fileName)
{
    if (std::filesystem::remove(fileName))
    {
        std::cout << "File deleted successfully." << std::endl;
    }
    else
    {
        std::cout << "Error deleting file or file not found." << std::endl;
    }
}

void renameFile(const std::string &oldName, const std::string &newName)
{
    try
    {
        std::filesystem::rename(oldName, newName);
        std::cout << "File renamed successfully." << std::endl;
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cout << "Error renaming file: " << e.what() << std::endl;
    }
}

void moveFile(const std::string &fileName, const std::string &directory)
{
    std::filesystem::path target = std::filesystem::path(directory) / std::filesystem::path(fileName).filename();
    try
    {
        std::filesystem::rename(fileName, target);
        std::cout << "File moved successfully." << std::endl;
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cout << "Error moving file: " << e.what() << std::endl;
    }
}

void copyFile(const std::string &fileName, const std::string &directory)
{
    std::filesystem::path target = std::filesystem::path(directory) / std::filesystem::path(fileName).filename();
    try
    {
        std::filesystem::copy(fileName, target);
        std::cout << "File copied successfully." << std::endl;
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cout << "Error copying file: " << e.what() << std::endl;
    }
}

void createDirectory(const std::string &directoryName)
{
    if (std::filesystem::create_directory(directoryName))
    {
        std::cout << "Directory created successfully." << std::endl;
    }
    else
    {
        std::cout << "Error creating directory." << std::endl;
    }
}

void removeDirectory(const std::string &directoryName)
{
    bool hasContents = std::filesystem::directory_iterator(directoryName) != std::filesystem::directory_iterator();
    if (hasContents)
    {
        std::cout << "Warning: The directory contains files or subdirectories!" << std::endl;
        std::cout << "Are you sure you want to delete all contents and the directory? (y/n): ";

        char choice;
        std::cin >> choice;

        if (choice != 'y' && choice != 'Y')
        {
            std::cout << "Operation aborted." << std::endl;
            return; // exit the function without deleting
        }
    }

    try
    {
        std::filesystem::remove_all(directoryName);
        std::cout << "Directory and its contents removed successfully." << std::endl;
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cout << "Error removing directory: " << e.what() << std::endl;
    }
}

void changeDirctory(const std::string &directoryName)
{
    try
    {
        fs::current_path(directoryName);
        std::cout << "Changed directory to " << directoryName << "." << std::endl;
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cout << "Error changing directory: " << e.what() << std::endl;
    }
}

void listDirectory()
{
    try
    {
        std::string path = fs::current_path().string();
        for (const auto &entry : fs::directory_iterator(path))
        {
            std::cout << entry.path().filename() << std::endl;
        }
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cout << "Error listing directory: " << e.what() << std::endl;
    }
}


int main()
{
    // Enable Virtual Terminal Processing
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    std::cout << "Welcome to Simple File Manager!" << std::endl;
    std::cout << "Type 'help' for a list of commands." << std::endl;

    while (true)
    {
        // Fetch user name and PC name from environment variables
        const char *userName = std::getenv("USERNAME");
        const char *computerName = std::getenv("computerName");

        if (!userName || !computerName)
        {
            std::cerr << "Failed to get user or computer name." << std::endl;
            return 1;
        }

        // Get current path and replace backslashes with forward slashes
        std::string path = fs::current_path().string();
        path = ReplaceAll(path, "\\", "/");

        // Print the formatted output
        std::cout << "\033[32m" << userName << "@" << computerName << ":"
                  << "\033[0m"
                  << "\033[33m" << path << "> "
                  << "\033[0m";

        std::string input;
        std::getline(std::cin, input);

        if (input == "help")
        {
            displayHelp();
        }
        else if (input.substr(0, 6) == "create")
        {
            std::string fileName = input.substr(7);
            createFile(fileName);
        }
        else if (input.substr(0, 6) == "delete")
        {
            std::string fileName = input.substr(7);
            deleteFile(fileName);
        }
        else if (input.substr(0, 6) == "rename")
        {
            std::istringstream iss(input.substr(7));
            std::string oldName, newName;
            iss >> oldName >> newName;
            renameFile(oldName, newName);
        }
        else if (input.substr(0, 4) == "move")
        {
            std::istringstream iss(input.substr(5));
            std::string fileName, directory;
            iss >> fileName >> directory;
            moveFile(fileName, directory);
        }
        else if (input.substr(0, 4) == "copy")
        {
            std::istringstream iss(input.substr(5));
            std::string fileName, directory;
            iss >> fileName >> directory;
            copyFile(fileName, directory);
        }
        else if (input.substr(0, 5) == "mkdir")
        {
            std::string directoryName = input.substr(6);
            createDirectory(directoryName);
        }
        else if (input.substr(0, 5) == "rmdir")
        {
            std::string directoryName = input.substr(6);
            removeDirectory(directoryName);
        }
        else if (input.substr(0, 2) == "cd")
        {
            std::string directoryName = input.substr(3);
            changeDirctory(directoryName);
        }
        else if (input.substr(0, 2) == "ls")
        {
            listDirectory();
        }

        else
        {
            std::cout << "Invalid command. Type 'help' for a list of commands." << std::endl;
        }
    }

    return 0;
}