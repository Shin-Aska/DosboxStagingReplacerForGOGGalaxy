# DosboxStagingReplacerForGOGGalaxy
Replaces Dosbox with Dosbox staging on GOG Galaxy installed classic games

```sql
SELECT 
    type, name, tbl_name, rootpage
FROM 
    sqlite_schema
WHERE 
    type ='table' AND 
    name NOT LIKE 'sqlite_%';
```