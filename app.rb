#!/usr/bin/ruby
$LOAD_PATH.push("./ELF")
require "elf"
require "reader"
require "linker"

if ARGV.length < 1
  puts "input object file."
  exit 1
end

linker = ELF::Linker.new
elf_ojects = []

elf_class = nil
ARGV.each do |filepath|
  elf_object = ELF::Reader.new
  elf_object.read(filepath)
  if elf_class.nil?
    elf_class = elf_object.elf_class
  else
    throw "Different Elf Class found!" if elf_class != elf_object.elf_class
  end
  elf_object.section_h_map.delete_if { |key, val| val[:size] == 0 }
  elf_ojects << elf_object
end

# Link Object files.
linker.link "linked.o", elf_ojects
