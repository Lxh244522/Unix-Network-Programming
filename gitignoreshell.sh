find -type f -exec file {} + | grep "ELF*" | sed 's/^.\///g' | awk '{print $1}' | sed 's/://g' >> .gitignore
