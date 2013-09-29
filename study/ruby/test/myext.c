#include "ruby.h"
#include "ruby/intern.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

VALUE MyExt = Qnil;

VALUE method_myext1(VALUE self,VALUE vi) {
  int i = NUM2INT(vi);
  printf("test1 (%d) start\n",i);
  fflush(stdout);
  sleep(1);
  printf("test1 end\n");
  fflush(stdout);

  return Qnil;
}

VALUE method_myext2(VALUE self) {
  printf("TEST2 START\n");
  fflush(stdout);
  sleep(3);
  printf("TEST2 END       **** HERE ****\n");
  fflush(stdout);
  return Qnil;
}

#define BLOCKING_REGION(func, arg) (long)rb_thread_blocking_region((func), (arg), RUBY_UBF_IO, 0)
VALUE mysleep(void *a){
  sleep(3);
  return 0;
}
VALUE method_myext3(VALUE self) {
  printf("TEST3 START\n");
  fflush(stdout);
  (void)BLOCKING_REGION(mysleep, 0);
  printf("TEST3 END       **** HERE ****\n");
  fflush(stdout);
  return Qnil;
}

void Init_myext() {
  MyExt = rb_define_module("MyExt");
  rb_define_method(MyExt, "myext1", method_myext1, 1);
  rb_define_method(MyExt, "myext2", method_myext2, 0);
  rb_define_method(MyExt, "myext3", method_myext3, 0);
}
