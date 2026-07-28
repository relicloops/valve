
---
name: plans
description: Plans and Tasks conventions and rules for the codebase. Path-scoped to plans and tasks.
paths:
  - ".prompt/plans/**/*.md"
  - ".prompt/tasks/**/*.md"
---

# Plans and Tasks rules

Plan rules translate to `PR[ID<int>]`.  
This may override a per project rule.  
When assigning a task or plan we can override the plans rules in the prompt.

Active plans live in `.prompt/plans/`.  
Move completed plans to `.prompt/refs/plans/done/`;  
keep that directory in the repository even when it contains no completed plans.

*an example usage.*
```text
[...] plan prompt
[...] we need to override PR[362] cause it blocks the execution of the plan
[...] plan prompt
```

___

( _start rules list_ )

___

## PR000 [ not implemented yet stays here as a placeholder for future rules ]

( _empty placeholder rule <PR[ID000]>_ )

___

## PR001 [ not implemented yet stays here as a placeholder for future rules ]

( _empty placeholder rule <PR[ID001]>_ )

___

( _end rules list_ )

___

## other codebase rules.

### pseudo-plan

The pseudo-plan are `user` requested plan not called from the agent harness tool.
- they must have a front-matter with fields:
  - [  
      name -> short name,  
      description -> a description max 100 chars,  
      severity -> high|critical|low...and so on,  
      status -> what is done from .check or plan specific  
    ]  
- saved in the default directory `.prompt/plans/[name].pseudo.md`
- not required to be executed but agents can give a notice to the user if them are not marked complete and also if them are marked high|critical severity.

### pseudo-task

The pseudo-task are `user` requested tasks.
- they must have a front-matter with fields:
  - [  
      name -> short name,  
      description -> a description max 100 chars,  
      severity -> high|critical|low...and so on,  
      status -> what is done from .check or plan specific  
    ]  
- saved in the default directory `.prompt/tasks/[name].pseudo.md`
- not required to be executed but agents can give a notice to the user if them are not marked complete and also if them are marked high|critical severity.
- they must have a todo check list:

```text
# title 

description

## todo check list

[a brief workflow description]

list of todo checks
  - [] check0001
  - [] and so n
```
- the agent harness must show the task list progress too.

### Conventions

[ not implemented yet ]
