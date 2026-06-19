#pragma once 

// ============================================================================
// feature.h — Master Include File
// ============================================================================
// This file includes ALL feature headers.
// Just #include "Feature/feature.h" in main.cpp to get everything.
//
// Add new #include lines here as you create new feature files.
// ============================================================================

// --- File & Directory Management (Role 2) ---
#include "file.h"           // File operations (partially implemented ✅)
#include "directory.h"      // Directory operations (empty stub)
#include "navigation.h"     // cd, pwd, dir (empty stub)

// --- Process Management (Role 3 — Most important for OS course!) ---
#include "process.h"        // Process create/kill/list/suspend/resume + CTRL+C handler

// --- Environment Variables (Role 4) ---
#include "environment.h"    // Environment variables: path, addpath, set_env

// --- System & Shell Utilities (Role 4) ---
#include "system_utils.h"   // time, date, .bat file execution
#include "help.h"           // Help system — documents all commands

// --- Command History (Role 1) ---
#include "history.h"        // Command history