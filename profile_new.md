$ pytest tests/profile_ipc23lt.py  -x
========================================================================== test session starts ===========================================================================
platform linux -- Python 3.12.3, pytest-8.2.1, pluggy-1.6.0
rootdir: /home/dzc/code/work/wlplan
configfile: pytest.ini
plugins: isort-4.0.0, black-0.6.0
collected 10 items                                                                                                                                                       

tests/profile_ipc23lt.py::test_profile[blocksworld] 
----------------------------------------------------------------------------- live log call ------------------------------------------------------------------------------
[2025-06-03 17:30:53 __init__.py::99 INFO] blocksworld predicates for keep_statics=False: [arm-empty/0, clear/1, holding/1, on-table/1, on/2]
[2025-06-03 17:31:01 __init__.py::99 INFO] blocksworld predicates for keep_statics=False: [arm-empty/0, clear/1, holding/1, on-table/1, on/2]
[2025-06-03 17:31:09 util.py::20 INFO] config  n_data  n_feat    time  
[2025-06-03 17:31:09 util.py::20 INFO]    set    1348   10445  0.3677  
[2025-06-03 17:31:09 util.py::20 INFO]   mset    1348   10445  0.3600  
PASSED                                                                                                                                                             [ 10%]
tests/profile_ipc23lt.py::test_profile[childsnack] 
----------------------------------------------------------------------------- live log call ------------------------------------------------------------------------------
[2025-06-03 17:31:09 __init__.py::99 INFO] childsnack predicates for keep_statics=False: [at/2, at_kitchen_bread/1, at_kitchen_content/1, at_kitchen_sandwich/1, no_gluten_sandwich/1, notexist/1, ontray/2, served/1]
[2025-06-03 17:31:12 __init__.py::99 INFO] childsnack predicates for keep_statics=False: [at/2, at_kitchen_bread/1, at_kitchen_content/1, at_kitchen_sandwich/1, no_gluten_sandwich/1, notexist/1, ontray/2, served/1]
[2025-06-03 17:31:14 util.py::20 INFO] config  n_data  n_feat    time  
[2025-06-03 17:31:14 util.py::20 INFO]    set     510     133  0.0103  
[2025-06-03 17:31:14 util.py::20 INFO]   mset     510     255  0.0125  
PASSED                                                                                                                                                             [ 20%]
tests/profile_ipc23lt.py::test_profile[ferry] 
----------------------------------------------------------------------------- live log call ------------------------------------------------------------------------------
[2025-06-03 17:31:14 __init__.py::99 INFO] ferry predicates for keep_statics=False: [at-ferry/1, at/2, empty-ferry/0, on/1]
[2025-06-03 17:31:19 __init__.py::99 INFO] ferry predicates for keep_statics=False: [at-ferry/1, at/2, empty-ferry/0, on/1]
[2025-06-03 17:31:25 util.py::20 INFO] config  n_data  n_feat    time  
[2025-06-03 17:31:25 util.py::20 INFO]    set    1434     550  0.0518  
[2025-06-03 17:31:25 util.py::20 INFO]   mset    1434    3232  0.1428  
PASSED                                                                                                                                                             [ 30%]
tests/profile_ipc23lt.py::test_profile[floortile] 
----------------------------------------------------------------------------- live log call ------------------------------------------------------------------------------
[2025-06-03 17:31:25 __init__.py::99 INFO] floortile predicates for keep_statics=False: [clear/1, painted/2, robot-at/2, robot-has/2]
[2025-06-03 17:31:32 __init__.py::99 INFO] floortile predicates for keep_statics=False: [clear/1, painted/2, robot-at/2, robot-has/2]
[2025-06-03 17:31:45 util.py::20 INFO] config  n_data  n_feat    time  
[2025-06-03 17:31:45 util.py::20 INFO]    set    3251     254  0.1933  
[2025-06-03 17:31:45 util.py::20 INFO]   mset    3251    7654  0.7788  
PASSED                                                                                                                                                             [ 40%]
tests/profile_ipc23lt.py::test_profile[miconic] 
----------------------------------------------------------------------------- live log call ------------------------------------------------------------------------------
[2025-06-03 17:31:45 __init__.py::99 INFO] miconic predicates for keep_statics=False: [boarded/1, lift-at/1, origin/2, served/1]
[2025-06-03 17:31:52 __init__.py::99 INFO] miconic predicates for keep_statics=False: [boarded/1, lift-at/1, origin/2, served/1]
[2025-06-03 17:31:59 util.py::20 INFO] config  n_data  n_feat    time  
[2025-06-03 17:31:59 util.py::20 INFO]    set    1729      66  0.0333  
[2025-06-03 17:31:59 util.py::20 INFO]   mset    1729     112  0.0381  
PASSED                                                                                                                                                             [ 50%]
tests/profile_ipc23lt.py::test_profile[rover] 
----------------------------------------------------------------------------- live log call ------------------------------------------------------------------------------
[2025-06-03 17:32:00 __init__.py::99 INFO] rover predicates for keep_statics=False: [at/2, at_rock_sample/1, at_soil_sample/1, calibrated/2, communicated_image_data/2, communicated_rock_data/1, communicated_soil_data/1, empty/1, full/1, have_image/3, have_rock_analysis/2, have_soil_analysis/2]
[2025-06-03 17:32:08 __init__.py::99 INFO] rover predicates for keep_statics=False: [at/2, at_rock_sample/1, at_soil_sample/1, calibrated/2, communicated_image_data/2, communicated_rock_data/1, communicated_soil_data/1, empty/1, full/1, have_image/3, have_rock_analysis/2, have_soil_analysis/2]
[2025-06-03 17:32:23 util.py::20 INFO] config  n_data  n_feat    time  
[2025-06-03 17:32:23 util.py::20 INFO]    set    1495    9618  0.3721  
[2025-06-03 17:32:23 util.py::20 INFO]   mset    1495   24551  0.8682  
PASSED                                                                                                                                                             [ 60%]
tests/profile_ipc23lt.py::test_profile[satellite] 
----------------------------------------------------------------------------- live log call ------------------------------------------------------------------------------
[2025-06-03 17:32:23 __init__.py::99 INFO] satellite predicates for keep_statics=False: [calibrated/1, have_image/2, pointing/2, power_avail/1, power_on/1]
[2025-06-03 17:32:32 __init__.py::99 INFO] satellite predicates for keep_statics=False: [calibrated/1, have_image/2, pointing/2, power_avail/1, power_on/1]
[2025-06-03 17:32:46 util.py::20 INFO] config  n_data  n_feat    time  
[2025-06-03 17:32:46 util.py::20 INFO]    set    1248    6989  0.2527  
[2025-06-03 17:32:46 util.py::20 INFO]   mset    1248   23025  0.6899  
PASSED                                                                                                                                                             [ 70%]
tests/profile_ipc23lt.py::test_profile[sokoban] 
----------------------------------------------------------------------------- live log call ------------------------------------------------------------------------------
[2025-06-03 17:32:46 __init__.py::99 INFO] sokoban predicates for keep_statics=False: [at-robot/1, at/2, clear/1]
[2025-06-03 17:32:57 __init__.py::99 INFO] sokoban predicates for keep_statics=False: [at-robot/1, at/2, clear/1]
[2025-06-03 17:33:08 util.py::20 INFO] config  n_data  n_feat    time  
[2025-06-03 17:33:08 util.py::20 INFO]    set    2521      96  0.2839  
[2025-06-03 17:33:08 util.py::20 INFO]   mset    2521      96  0.3200  
PASSED                                                                                                                                                             [ 80%]
tests/profile_ipc23lt.py::test_profile[spanner] 
----------------------------------------------------------------------------- live log call ------------------------------------------------------------------------------
[2025-06-03 17:33:08 __init__.py::99 INFO] spanner predicates for keep_statics=False: [at/2, carrying/2, loose/1, tightened/1, usable/1]
[2025-06-03 17:33:14 __init__.py::99 INFO] spanner predicates for keep_statics=False: [at/2, carrying/2, loose/1, tightened/1, usable/1]
[2025-06-03 17:33:21 util.py::20 INFO] config  n_data  n_feat    time  
[2025-06-03 17:33:21 util.py::20 INFO]    set    1293     117  0.0432  
[2025-06-03 17:33:21 util.py::20 INFO]   mset    1293     539  0.0642  
PASSED                                                                                                                                                             [ 90%]
tests/profile_ipc23lt.py::test_profile[transport] 
----------------------------------------------------------------------------- live log call ------------------------------------------------------------------------------
[2025-06-03 17:33:21 __init__.py::99 INFO] transport predicates for keep_statics=False: [at/2, capacity/2, in/2]
[2025-06-03 17:33:24 __init__.py::99 INFO] transport predicates for keep_statics=False: [at/2, capacity/2, in/2]
[2025-06-03 17:33:29 util.py::20 INFO] config  n_data  n_feat    time  
[2025-06-03 17:33:29 util.py::20 INFO]    set     782     529  0.0294  
[2025-06-03 17:33:29 util.py::20 INFO]   mset     782    3844  0.0919  
PASSED                                                                                                                                                             [100%]

===================================================================== 10 passed in 155.37s (0:02:35) =====================================================================

