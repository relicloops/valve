bool verb_name_valid_(const char *name) {
  return name && name[0] != '\0' && name[0] != '-';
}