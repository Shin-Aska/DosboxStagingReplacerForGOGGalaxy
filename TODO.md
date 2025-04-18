# Core Functionality

- [X] Fix the header and body structure to simplify importing
- [x] Add a way to find all installed windows programs (preferably using Win32 API)
- [x] Add a way to verify if a file exists
  - [ ] Can determine if it has UAC permission
- [x] Add a way to list files and folders in a directory
- [x] Add a way to read SQLITE3 files (using external libraries is ok)
- [x] Add SQLite procedures for adding custom configurations
  - Currently I found out that PlayTasks is my area of interests
    - Other tables include PlayTaskTypes, PlayTaskLaunchParameters
    - PlayTasks uses a many to many approach with Products (Information is at Details and LimitedDetails table and Product Details View)
      - The mapping is at ProductsToReleaseKeys where gogId is the ID in the Product Detail (used by Details and LimitedDetails tables)
      - The execution order will be
        - Get all users in the User table and iterate for each row and cross check it with UserReleaseProperties if such mapping exist for the Product chosen
          - Set all isPrimary to 0 for all instances
          - Find all instances of the product under playTask and keep it from memory (using the chosen gameReleaseKey as reference)
          - insert a new row, with the following values:
            - column will be `MAX(order) + 1`
            - userId will be the current Id of the user instance ***TODO: This may not be needed!***
            - type will be Custom (Query the PlayTaskTypes to be sure to determine the value, seems to be hardcoded as 85 but don't rely on this to be sure!)
            - isPrimary is 1
          - Set the previously inserted row's isPrimary to 1
    - When adding PlayTasks, we must ensure that customLaunchParameters under ProductSettings is set to 1
- [x] Batch file handling where the application modifies the batch file to align with the current adjustments

# CLI
- [x] Ability to add a special dosbox build as a custom launch parameter
- [x] Make the added custom launch parameter a default option

# Unit Testing
- [ ] Add unit tests for the core functionality
- [ ] Add unit tests for the CLI