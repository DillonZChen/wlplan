$ pytest tests/profile_ipc23lt.py 
========================================================================== test session starts ===========================================================================
platform linux -- Python 3.12.3, pytest-8.2.1, pluggy-1.6.0
rootdir: /home/dzc/code/work/wlplan
configfile: pytest.ini
plugins: isort-4.0.0, black-0.6.0
collected 10 items                                                                                                                                                       

tests/profile_ipc23lt.py::test_profile[blocksworld] 
----------------------------------------------------------------------------- live log call ------------------------------------------------------------------------------
[2025-06-03 16:28:40 __init__.py::96 INFO] blocksworld predicates for keep_statics=False: [arm-empty/0, clear/1, holding/1, on-table/1, on/2]
[2025-06-03 16:28:44 __init__.py::96 INFO] blocksworld predicates for keep_statics=False: [arm-empty/0, clear/1, holding/1, on-table/1, on/2]
[2025-06-03 16:28:49 util.py::20 INFO] config  n_data  n_feat    time  
[2025-06-03 16:28:49 util.py::20 INFO]    set    1348   10445  0.3644  
[2025-06-03 16:28:49 util.py::20 INFO]   mset    1348   10445  0.3654  
PASSED                                                                                                                                                             [ 10%]
tests/profile_ipc23lt.py::test_profile[childsnack] 
----------------------------------------------------------------------------- live log call ------------------------------------------------------------------------------
[2025-06-03 16:28:49 __init__.py::96 INFO] childsnack predicates for keep_statics=False: [at/2, at_kitchen_bread/1, at_kitchen_content/1, at_kitchen_sandwich/1, no_gluten_sandwich/1, notexist/1, ontray/2, served/1]
[2025-06-03 16:28:49 __init__.py::96 INFO] childsnack predicates for keep_statics=False: [at/2, at_kitchen_bread/1, at_kitchen_content/1, at_kitchen_sandwich/1, no_gluten_sandwich/1, notexist/1, ontray/2, served/1]
[2025-06-03 16:28:49 util.py::20 INFO] config  n_data  n_feat    time  
[2025-06-03 16:28:49 util.py::20 INFO]    set     510     133  0.0116  
[2025-06-03 16:28:49 util.py::20 INFO]   mset     510     255  0.0136  
PASSED                                                                                                                                                             [ 20%]
tests/profile_ipc23lt.py::test_profile[ferry] 
----------------------------------------------------------------------------- live log call ------------------------------------------------------------------------------
[2025-06-03 16:28:49 __init__.py::96 INFO] ferry predicates for keep_statics=False: [at-ferry/1, at/2, empty-ferry/0, on/1]
[2025-06-03 16:28:50 __init__.py::96 INFO] ferry predicates for keep_statics=False: [at-ferry/1, at/2, empty-ferry/0, on/1]
[2025-06-03 16:28:52 util.py::20 INFO] config  n_data  n_feat    time  
[2025-06-03 16:28:52 util.py::20 INFO]    set    1434     550  0.0524  
[2025-06-03 16:28:52 util.py::20 INFO]   mset    1434    3232  0.1466  
PASSED                                                                                                                                                             [ 30%]
tests/profile_ipc23lt.py::test_profile[floortile] 
----------------------------------------------------------------------------- live log call ------------------------------------------------------------------------------
[2025-06-03 16:28:52 __init__.py::96 INFO] floortile predicates for keep_statics=False: [clear/1, painted/2, robot-at/2, robot-has/2]
[2025-06-03 16:28:55 __init__.py::96 INFO] floortile predicates for keep_statics=False: [clear/1, painted/2, robot-at/2, robot-has/2]
[2025-06-03 16:29:04 util.py::20 INFO] config  n_data  n_feat    time  
[2025-06-03 16:29:04 util.py::20 INFO]    set    3251     254  0.2331  
[2025-06-03 16:29:04 util.py::20 INFO]   mset    3251    7654  0.8304  
PASSED                                                                                                                                                             [ 40%]
tests/profile_ipc23lt.py::test_profile[miconic] 
----------------------------------------------------------------------------- live log call ------------------------------------------------------------------------------
[2025-06-03 16:29:04 __init__.py::96 INFO] miconic predicates for keep_statics=False: [boarded/1, lift-at/1, origin/2, served/1]
[2025-06-03 16:29:05 __init__.py::96 INFO] miconic predicates for keep_statics=False: [boarded/1, lift-at/1, origin/2, served/1]
[2025-06-03 16:29:06 util.py::20 INFO] config  n_data  n_feat    time  
[2025-06-03 16:29:06 util.py::20 INFO]    set    1729      66  0.0335  
[2025-06-03 16:29:06 util.py::20 INFO]   mset    1729     112  0.0387  
PASSED                                                                                                                                                             [ 50%]
tests/profile_ipc23lt.py::test_profile[rovers] 
----------------------------------------------------------------------------- live log call ------------------------------------------------------------------------------
[2025-06-03 16:29:06 __init__.py::96 INFO] rovers predicates for keep_statics=False: [at/2, at_rock_sample/1, at_soil_sample/1, calibrated/2, communicated_image_data/2, communicated_rock_data/1, communicated_soil_data/1, empty/1, full/1, have_image/3, have_rock_analysis/2, have_soil_analysis/2]
[2025-06-03 16:29:10 __init__.py::96 INFO] rovers predicates for keep_statics=False: [at/2, at_rock_sample/1, at_soil_sample/1, calibrated/2, communicated_image_data/2, communicated_rock_data/1, communicated_soil_data/1, empty/1, full/1, have_image/3, have_rock_analysis/2, have_soil_analysis/2]
[2025-06-03 16:29:21 util.py::20 INFO] config  n_data  n_feat    time  
[2025-06-03 16:29:21 util.py::20 INFO]    set    1495    9618  0.3814  
[2025-06-03 16:29:21 util.py::20 INFO]   mset    1495   24551  0.9355  
PASSED                                                                                                                                                             [ 60%]
tests/profile_ipc23lt.py::test_profile[satellite] 
----------------------------------------------------------------------------- live log call ------------------------------------------------------------------------------
[2025-06-03 16:29:21 __init__.py::96 INFO] satellite predicates for keep_statics=False: [calibrated/1, have_image/2, pointing/2, power_avail/1, power_on/1]
[2025-06-03 16:29:24 __init__.py::96 INFO] satellite predicates for keep_statics=False: [calibrated/1, have_image/2, pointing/2, power_avail/1, power_on/1]
[2025-06-03 16:29:33 util.py::20 INFO] config  n_data  n_feat    time  
[2025-06-03 16:29:33 util.py::20 INFO]    set    1248    6989  0.2798  
[2025-06-03 16:29:33 util.py::20 INFO]   mset    1248   23025  0.7865  
PASSED                                                                                                                                                             [ 70%]
tests/profile_ipc23lt.py::test_profile[sokoban] 
----------------------------------------------------------------------------- live log call ------------------------------------------------------------------------------
[2025-06-03 16:29:33 __init__.py::96 INFO] sokoban predicates for keep_statics=False: [at-robot/1, at/2, clear/1]
[2025-06-03 16:29:38 __init__.py::96 INFO] sokoban predicates for keep_statics=False: [at-robot/1, at/2, clear/1]
[2025-06-03 16:29:44 util.py::20 INFO] config  n_data  n_feat    time  
[2025-06-03 16:29:44 util.py::20 INFO]    set    2521      96  0.4125  
[2025-06-03 16:29:44 util.py::20 INFO]   mset    2521      96  0.4294  
PASSED                                                                                                                                                             [ 80%]
tests/profile_ipc23lt.py::test_profile[spanner] 
----------------------------------------------------------------------------- live log call ------------------------------------------------------------------------------
[2025-06-03 16:29:44 __init__.py::96 INFO] spanner predicates for keep_statics=False: [at/2, carrying/2, loose/1, tightened/1, usable/1]
[2025-06-03 16:29:45 __init__.py::96 INFO] spanner predicates for keep_statics=False: [at/2, carrying/2, loose/1, tightened/1, usable/1]
[2025-06-03 16:29:46 util.py::20 INFO] config  n_data  n_feat    time  
[2025-06-03 16:29:46 util.py::20 INFO]    set    1293     117  0.0548  
[2025-06-03 16:29:46 util.py::20 INFO]   mset    1293     539  0.0755  
PASSED                                                                                                                                                             [ 90%]
tests/profile_ipc23lt.py::test_profile[transport] 
----------------------------------------------------------------------------- live log call ------------------------------------------------------------------------------
[2025-06-03 16:29:46 __init__.py::96 INFO] transport predicates for keep_statics=False: [at/2, capacity/2, in/2]
[2025-06-03 16:29:46 __init__.py::96 INFO] transport predicates for keep_statics=False: [at/2, capacity/2, in/2]
[2025-06-03 16:29:48 util.py::20 INFO] config  n_data  n_feat    time  
[2025-06-03 16:29:48 util.py::20 INFO]    set     782     529  0.0335  
[2025-06-03 16:29:48 util.py::20 INFO]   mset     782    3844  0.1058  
PASSED                                                                                                                                                             [100%]

===================================================================== 10 passed in 67.49s (0:01:07) ======================================================================

