#include <stdio.h>
#include <stdlib.h>

struct mab {
  int offset;
  int size;
  int allocated;
  struct mab *next;
  struct mab *prev;
};

typedef struct mab Mab;
typedef Mab *MabPtr;

void printMemory(MabPtr m) {
  printf("\n--- Memory Layout ---\n");
  for (MabPtr current = m; current; current = current->next) {
    printf("Offset: %d, Size: %d, Status: %s\n", current->offset, current->size,
           current->allocated ? "Allocated" : "Free");
  }
  printf("---------------------\n\n");
}

MabPtr memChk(MabPtr m, int size) {
  for (MabPtr current = m; current; current = current->next) {
    if (!current->allocated && current->size >= size) {
      return current;
    }
  }
  return NULL;
}

MabPtr memSplit(MabPtr m, int size) {
  if (m && m->size > size) {
    MabPtr newBlock = (MabPtr)malloc(sizeof(Mab));
    if (!newBlock) {
      perror("Failed to allocate memory for split");
      exit(EXIT_FAILURE);
    }
    *newBlock = (Mab){.offset = m->offset + size,
                      .size = m->size - size,
                      .allocated = 0,
                      .next = m->next,
                      .prev = m};

    if (m->next) {
      m->next->prev = newBlock;
    }
    m->next = newBlock;
    m->size = size;
    printf("Split block. New free block of size %d created.\n", newBlock->size);
  }
  return m;
}

MabPtr memAlloc(MabPtr m, int size) {
  MabPtr block = memChk(m, size);
  if (block) {
    if (block->size > size) {
      block = memSplit(block, size);
    }
    block->allocated = 1;
    printf("Allocated block of size %d at offset %d\n", block->size,
           block->offset);
  } else {
    printf("Allocation failed for size %d\n", size);
  }
  return m;
}

MabPtr memMerge(MabPtr m) {
  if (m->next && !m->next->allocated) {
    MabPtr temp = m->next;
    m->size += temp->size;
    m->next = temp->next;
    if (m->next) {
      m->next->prev = m;
    }
    free(temp);
    printf("Merged with next block.\n");
  }
  if (m->prev && !m->prev->allocated) {
    MabPtr prevBlock = m->prev;
    prevBlock->size += m->size;
    prevBlock->next = m->next;
    if (m->next) {
      m->next->prev = prevBlock;
    }
    free(m);
    printf("Merged with previous block.\n");
    return prevBlock;
  }
  return m;
}

MabPtr memFree(MabPtr m) {
  if (m && m->allocated) {
    m->allocated = 0;
    printf("Freed block of size %d at offset %d\n", m->size, m->offset);
    m = memMerge(m);
  }
  return m;
}

int main() {
  MabPtr memory = (MabPtr)malloc(sizeof(Mab));
  if (!memory) {
    perror("Failed to initialize memory");
    return EXIT_FAILURE;
  }
  *memory = (Mab){
      .offset = 0, .size = 1024, .allocated = 0, .next = NULL, .prev = NULL};

  printMemory(memory);

  memory = memAlloc(memory, 100);
  printMemory(memory);
  memory = memAlloc(memory, 250);
  printMemory(memory);
  memory = memAlloc(memory, 300);
  printMemory(memory);

  MabPtr blockToFree = memory->next;
  memFree(blockToFree);
  printMemory(memory);

  memory = memAlloc(memory, 50);
  printMemory(memory);

  for (MabPtr current = memory, next; current; current = next) {
    next = current->next;
    free(current);
  }

  return EXIT_SUCCESS;
}
