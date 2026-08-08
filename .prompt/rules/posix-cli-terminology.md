---
name: POSIX CLI terminology
description: Standard vocabulary for command-line parsing concepts.
---

# POSIX CLI Terminology

The visual, multi-page version of this audit is the [Valve parser road-map](../refs/road-map/index.html).

Standard vocabulary for command-line parsing concepts, drawn from POSIX
Chapter 12 (Utility Conventions) and GNU conventions.

## POSIX core terms

| Term                | Definition                                                  | Example                                |
|---------------------|-------------------------------------------------------------|----------------------------------------|
| **option**          | A flag that modifies utility behavior                       | ``-f``, ``--flag``                     |
| **short option**    | Single-character option prefixed with ``-``                 | ``-f``, ``-v``                         |
| **long option**     | Multi-character option prefixed with ``--`` (GNU extension) | ``--flag``, ``--verbose``              |
| **option-argument** | The value associated with an option                         | ``value`` in ``-f value``              |
| **operand**         | Positional argument after all options (see below)           | ``file.txt`` in ``cp -r src file.txt`` |

## Operands in detail

Operands are the non-option arguments a utility acts on. They are positional
-- their meaning comes from their position, not from a flag name. Everything
that is not an option or an option-argument is an operand.

```
cp  -r  src/  dest/
^^  ^^  ^^^^  ^^^^^
|   |   |     |
|   |   |     second operand (destination)
|   |   first operand (source)
|   option (-r = recursive)
utility name
```

POSIX Guideline 11 requires that all options precede operands on the command
line. The special token ``--`` explicitly ends option processing -- everything
after it is treated as an operand, even if it looks like an option:

```
rm -- -f          # removes a file literally named "-f"
grep -- --help .  # searches for the string "--help"
```

Common operand patterns across standard utilities:

| Utility | Operands | Meaning |
|---------|----------|---------|
| ``cp src dest`` | ``src``, ``dest`` | source file, destination |
| ``grep pattern file`` | ``pattern``, ``file`` | regex, input file |
| ``mv old new`` | ``old``, ``new`` | source path, target path |
| ``cat file1 file2`` | ``file1``, ``file2`` | files to concatenate |
| ``chmod 755 script`` | ``755``, ``script`` | mode, target file |

The key distinction: options are **named** (``-r``, ``--verbose``), operands
are **positional**. An option's meaning comes from its flag; an operand's
meaning comes from where it appears in the argument list.

valve does not support operands yet. Unprefixed non-option tokens are rejected
as unexpected positional arguments. ``VL_BEHAVIOR_ACCEPT_OPERANDS`` is
declared but not implemented, and the documented ``vl_operand_count()`` /
``vl_operand_at()`` accessors do not exist yet.

The current ``--`` branch stops parsing and silently ignores every remaining
token. It does not collect operands or reject the ignored tail. Correct POSIX
end-of-options behavior therefore remains incomplete.

### Operands as option shorthand

Once operand support exists, operands may serve as positional shorthand for
named options. The consumer would define the mapping by convention while the
parser collects raw strings in insertion order.

Example: a ``valve run`` command with named options and positional shorthand:

```
valve run --enable-log --enable-listening-port --path=./service filename.log 7890
```

Named options parsed by the library:

- ``--enable-log`` -> toggle
- ``--enable-listening-port`` -> toggle
- ``--path=./service`` -> string option

Operands that would be collected by the library (insertion order):

```
operand[0] = "filename.log"
operand[1] = "7890"
```

Consumer-side mapping (not library logic):

```
operand[0] -> "filename.log" -> short for --log-path
operand[1] -> "7890"         -> short for --listening-port
```

The library's job stops at collecting the operand array. The consumer owns
the positional contract: which index maps to which option, type validation,
and whether the order is correct. If the user swaps the operands, that is
between the user and the consumer's documentation -- same as every POSIX
utility.

## How the option-argument attaches

POSIX does not formally name these styles, but the community and GNU
documentation use:

| Style                                  | Description                                                   | Example                        |
|----------------------------------------|---------------------------------------------------------------|--------------------------------|
| **inline** / **attached** / **joined** | Value in the same argv element, joined with ``=`` or directly | ``--flag=value``, ``-ovalue``  |
| **separate** / **detached**            | Value in the next argv element                                | ``--flag value``, ``-o value`` |

POSIX Guideline 7: option-arguments should not be optional. If an option
does require an option-argument, conforming applications shall place it as
a separate argument. However, implementations must also permit the
option-argument in the same argument string (inline).

For optional option-arguments, the value must be inline (attached directly),
never separate -- otherwise the parser cannot distinguish the value from the
next option or operand.

valve currently selects inline or separate assignment globally. It supports
both styles across configurations, but one executable cannot accept both
styles for the same option at the same time.

## All option-argument forms

Complete list of syntactic forms for passing values to options:

| Form                 | Example                        | Description                                     |
|----------------------|--------------------------------|-------------------------------------------------|
| long inline (``=``)  | ``--flag=value``               | Long option with ``=`` separator                |
| long separate        | ``--flag value``               | Long option, value in next argv                 |
| long joined          | ``--flagvalue``                | Long option, value directly attached (no ``=``) |
| short inline (``=``) | ``-f=value``                   | Short option with ``=`` separator               |
| short separate       | ``-f value``                   | Short option, value in next argv                |
| short joined         | ``-fvalue``                    | Short option, value directly attached           |
| short stacking       | ``-abc``                       | Multiple flags in one token (``-a -b -c``)      |
| enable/disable       | ``--enable-X``/``--disable-X`` | Boolean toggle via prefix                       |
| dot notation         | ``--proxy.lane=fast``          | Hierarchical option names                       |
| operands             | ``file.txt``                   | Positional arguments (no prefix)                |

## Valve status -- 2026-08-08

| Capability | Status today | Current behavior | Missing implementation |
|---|---|---|---|
| POSIX/GNU API terminology | complete | ``VL_ASSIGN_*`` and ``VL_OPT_TYPE_*`` use the intended vocabulary | none |
| Long inline | complete, mode-specific | ``--flag=value`` works with ``VL_ASSIGN_INLINE`` | none within inline mode |
| Long separate | complete, mode-specific | ``--flag value`` works with ``VL_ASSIGN_SEPARATE`` | none within separate mode |
| Short inline | complete, mode-specific | ``-f=value`` works with ``VL_ASSIGN_INLINE`` | none within inline mode |
| Short separate | complete, mode-specific | ``-f value`` works with ``VL_ASSIGN_SEPARATE`` | none within separate mode |
| Inline and separate together | missing | Assignment mode is executable-global | Add a mode/policy that accepts both forms |
| Long joined | missing | ``--flagvalue`` is treated as one exact option name | Joined-value recognition and ambiguity rules |
| Short joined | missing | ``-fvalue`` is rejected after the short name | Joined short option-argument parsing |
| Short stacking | missing | ``-abc`` is not expanded | Cluster parsing, including value-taking final options |
| Bare booleans | complete extension | Bare bool means true; inline ``=true``/``=false`` is accepted | General optional option-arguments remain unsupported |
| Enable/disable toggles | complete extension | ``--enable-X`` and ``--disable-X`` map to one toggle option | none |
| Dotted option names | complete extension | Exact dotted matching, target-typed values, target population, grouped help and targeted group help | none |
| Scoped help targets | complete extension | Segments after the matched verb or sub-verb rejoin into the option name, so ``?verb.sub.group.leaf`` and ``?verb.group.leaf`` reach a dotted option; a trailing lone segment resolves as a group prefix (``?verb.group``) | Group help cards are still rendered tree-wide, not narrowed to the owning verb |
| Joined ``?target`` help | complete custom extension | Reserved parser resolves joined help targets | Standardization or removal remains a policy choice |
| Operands | declared, missing | Ordinary positional tokens are rejected | Operand storage, accessors, behavior flag wiring and lifecycle |
| ``--`` end marker | incorrect partial behavior | Parsing stops and the remaining argv tail is ignored | Collect tail as operands or reject it when operands are disabled |
| Required options | complete | ``.required`` is enforced across the active command chain and shown in help | none |
| Conflicting options | complete | Many-option, undirected, presence-based conflicts with automatic help | Constraint-graph satisfiability is intentionally absent |
| Conditional requirements | complete | Directed many-option ``.requires`` with ``requires``/``required by`` help | Predicate/value-based and one-of requirements are out of scope |
| Inferred option help | complete | Signature comes from type, value, target and assignment mode | ``.usage`` remains optional supplementary guidance |
| Verbs and sub-verbs | complete at fixed depth | Any number of verbs and sibling sub-verbs; parsing selects one verb and one nested sub-verb | Reject deeper schemas or add recursive parsing, active-path state and help |

### Missing parser implementation expense

Relative expense against Valve's current parser, ordered high to low:

| Rank | Missing capability | Expense | Main cost |
|---|---|---|---|
| 1 | Operands and correct ``--`` behavior | very high | Add public storage and accessors, ownership and lifecycle, positional collection, option-value disambiguation, end-of-options behavior and help |
| 2 | Short stacking (``-abc``) | high | Add per-character cluster parsing, per-member errors and duplicates, and rules for a value-taking final member; it also overlaps short joined syntax |
| 3 | Long joined (``--flagvalue``) | high | Resolve arbitrary option-name prefixes, exact-name precedence and ambiguous schemas before separating the joined value |
| 4 | Short joined (``-fvalue``) | medium | Split after the unambiguous short name, define boolean/value behavior, and coordinate with future stacking |
| 5 | Inline and separate together | low | Extend assignment policy, reuse both existing parse branches, and show both accepted signatures in inferred help |

Operands and short stacking are not option-argument attachment forms.
Restricting the comparison to missing attachment forms, long joined remains
the most expensive.

## Mapping to valve -- [DONE]

| POSIX / GNU term            | Old valve name                                    | Current valve name                         |
|-----------------------------|---------------------------------------------------|--------------------------------------------|
| inline assignment (``=``)   | ``VL_ASSIGNMENT_FORM_EQUALS``                     | ``VL_ASSIGN_INLINE``                       |
| separate assignment (space) | ``VL_ASSIGNMENT_FORM_NEXT``                       | ``VL_ASSIGN_SEPARATE``                     |
| long option                 | ``VL_FORM_LONG_EQUALS`` / ``VL_FORM_LONG_NEXT``   | ``VL_OPT_TYPE_LONG``                       |
| short option                | ``VL_FORM_SHORT_EQUALS`` / ``VL_FORM_SHORT_NEXT`` | ``VL_OPT_TYPE_SHORT``                      |
| enable/disable toggle       | ``VL_FORM_ENABLE_DISABLE``                        | ``VL_OPT_TYPE_TOGGLE``                     |
| KV long equals              | ``VL_FORM_KV_LONG_EQUALS``                        | removed (routed by ``VL_OPTION_VALUE_KV``) |
| KV long next                | ``VL_FORM_KV_LONG_NEXT``                          | removed (routed by ``VL_OPTION_VALUE_KV``) |

## Simplified design -- [DONE]

Global assignment style decides inline vs separate. Per-option flags decide
long, short, or toggle. Value type decides KV routing.

```c
typedef enum vl_assign {
  VL_ASSIGN_INLINE = 0,   // --flag=value, -f=value
  VL_ASSIGN_SEPARATE,     // --flag value, -f value
} vl_assign_t;

typedef unsigned vl_opt_type_t;

enum {
  VL_OPT_TYPE_LONG   = 1u << 0,  // --flag
  VL_OPT_TYPE_SHORT  = 1u << 1,  // -f
  VL_OPT_TYPE_TOGGLE = 1u << 2,  // --enable-flag / --disable-flag
};
```

Three orthogonal concerns, no redundant bits.

## References

- `POSIX Chapter 12: Utility Conventions <https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap12.html>`_
- `GNU Argument Syntax <https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html>`_
- `Parsing Long Options with getopt_long <https://ftp.gnu.org/old-gnu/Manuals/glibc-2.2.5/html_node/Getopt-Long-Options.html>`_
