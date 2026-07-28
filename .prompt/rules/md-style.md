---
name: md-style
description: Writing and Markdown conventions for Valve documentation.
paths:
  - "**/*.md"
---

# Markdown rules

## Write for people

- Lead with what the reader can understand or accomplish, not with an inventory of features or an explanation of the writing process.
- Use active voice and present tense.
- Address the reader as “you” in instructions. Name Valve or the program when describing software behavior.
- Write concise, precise, natural prose with restrained warmth. Preserve Valve's personality when it helps the reader.
- Prefer familiar words and concrete statements. Keep terminology consistent.
- Avoid agent-facing narration, repository-work commentary, filler, clichés, hype, needless qualifiers, and generic promotional language.
- Avoid words such as “simply,” “obviously,” and “easy” when they dismiss work the reader must perform.
- Do not use exclamation marks for artificial enthusiasm.

## Let the renderer wrap prose

- Keep each prose paragraph on one physical source line. Do not manually wrap paragraphs to a column width.
- Use a blank line only when starting a new semantic paragraph.
- When editing a manually wrapped paragraph, join the entire touched paragraph. Do not reflow unrelated paragraphs, sections, or files.
- Use exactly two trailing ASCII spaces before a newline only for an intentional rendered line break within the same paragraph.
- Do not use hard line breaks to control text width or responsiveness. Markdown renderers own soft wrapping for raw and rendered views.
- Preserve intentional two-space hard breaks. Do not remove them as trailing whitespace, even when a whitespace check reports them.
- These paragraph rules do not alter syntax-significant line boundaries in frontmatter, headings, lists, blockquotes, tables, badge groups, code fences, or raw blocks.

## Structure the document

- Use one H1 for the document title.
- Use sentence case for headings and do not skip heading levels.
- Make headings describe the content that follows. Do not repeat a heading in an unnecessary introductory sentence.
- Use numbered lists for sequences and bulleted lists for unordered, parallel items.
- Keep list items grammatically parallel. Use prose when a list does not improve scanning or comprehension.
- Use emphasis sparingly. Do not use bold as a substitute for clear structure.

## Format technical content

- Add an accurate language identifier to every fenced code block.
- Keep commands directly copyable. Put explanations outside a command block unless a comment is part of the example.
- Use inline code for identifiers, filenames, commands, options, environment variables, and literal values.
- Use descriptive link text. Use repository-relative links for files in this repository.
- Give images meaningful alt text. Use empty alt text only for decorative images.
- Prefer standard Markdown over raw HTML when Markdown can express the same structure.

## Edit with restraint

- Preserve authored meaning, technical facts, names, versions, paths, and commands.
- Change only the requested prose and the surrounding text required to keep it coherent.
- Do not autoformat, reflow, or tidy an entire Markdown file.
- Read the rendered structure and the raw source before considering an edit complete.
