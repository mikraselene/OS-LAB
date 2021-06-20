#ifndef LAB3_SRC_PFSTAT_PFSTAT_H_
#define LAB3_SRC_PFSTAT_PFSTAT_H_

struct page_info {
  unsigned long num_physpages;
  int free, locked, reserved, swapcache;
  int referenced, slab, private, uptodate;
  int dirty, active, writeback, mappedtodisk;
  unsigned long valid;
};

#endif  // LAB3_SRC_PFSTAT_PFSTAT_H_
