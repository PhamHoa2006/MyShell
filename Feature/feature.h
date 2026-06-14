#pragma once 

// ============================================================================
// feature.h — Master Include File
// ============================================================================
// This file includes ALL feature headers.
// Just #include "Feature/feature.h" in main.cpp to get everything.
//
// Add new #include lines here as you create new feature files.
// ============================================================================

// --- File & Directory Management ---
#include "file.h"           // File operations (partially implemented ✅)
#include "directory.h"      // Directory operations (empty stub)
#include "navigation.h"     // cd, pwd, dir (empty stub)

// --- Process Management (Most important for OS course!) ---
#include "process.h"        // Process create/kill/list/suspend/resume (empty stub)

// --- Environment & Variables ---
#include "environment.h"    // Environment variables (empty stub)

// --- System Utilities ---
#include "system_utils.h"   // time, date, cpuinfo, meminfo (empty stub)

// --- Shell Utilities ---
#include "history.h"        // Command history (empty stub)
#include "help.h"           // Help system (empty stub)