import shutil
import os

# Copy file
def make_file_copy(folder_path, source_filename, target_filename):
    """Copies the content of source_filename to target_filename within the specified folder_path."""
    # Construct full paths for the source and target files
    source_file_path = os.path.join(folder_path, source_filename)
    target_file_path = target_filename
    
    try:
        # Open the source file in read mode
        with open(source_file_path, 'r') as source_file:
            content = source_file.read()
        
        # Open the target file in write mode and write the content
        with open(target_file_path, 'w') as target_file:
            target_file.write(content)
        
        print(f"File '{source_file_path}' successfully copied to '{target_file_path}'.")
        return target_file_path
    except Exception as e:
        print(f"Error occurred: {e}")


# Chope file up to token
def delete_to_token(new_file, token):
  try:
    # Open the file and read the contents
    with open(new_file, 'r') as file:
        content = file.read()
    
    # Find the position of the first occurrence of '$LIST'
    index = content.find(token)
    
    if index != -1:
        # Extract everything from '$LIST' to the end of the file
        new_content = content[index:]
        
        # Write the new content back to the file
        with open(new_file, 'w') as file:
            file.write(new_content)
        print("File has been updated.")
    else:
        print("The string " + token + " was not found in the file.")
        
  except FileNotFoundError:
    print("The file does not exist.")
  except Exception as e:
    print(f"An error occurred: {e}")


# Split the element into two parts
def split_element(element):
    # Part 1: From '<' to ';' (inclusive)
    start_index = element.find('<')
    end_index = element.find(';') + 1  # Include the ';'
    part1 = element[start_index:end_index].strip()

    # Part 2: From the first '{' to the last '}' (exclusive)
    start_brace = element.find('{') + 1  # Start after the first '{'
    end_brace = element.rfind('}')  # Find the last '}'
    part2 = element[start_brace:end_brace].strip()

    return part1, part2   


# Delete file
def delete_file(filename):    
  try:
    # Remove the file
    os.remove(file_name)
    print(f"File '{file_name}' has been deleted successfully.")
  except FileNotFoundError:
    print(f"The file '{file_name}' does not exist.")
  except PermissionError:
    print("Permission denied: unable to delete the file.")
  except Exception as e:
    print(f"An error occurred: {e}")
  

# Split file
def split_file_to_elements(source_file):

  new_file = make_file_copy(".", source_file, source_file + ".copy")

  delete_to_token(new_file, '$LIST')
  delete_to_token(new_file, '*******************************************************************************')

  try:
    # Open the file and read the contents
    with open(new_file, 'r') as file:
        content = file.read()
    
    # Replace the target string
    new_content = content.replace('*------------------------------------------------------------------------------', 
                                  '*******************************************************************************')
    
    # Write the modified content back to the file
    with open(new_file, 'w') as file:
        file.write(new_content)
    print("All occurrences have been replaced successfully.")
    
    # Open the file and read the contents
    with open(new_file, 'r') as file:
        content = file.read()
    
    # Define the delimiter
    delimiter = '*******************************************************************************'
    
    # Split the content by the delimiter
    elements = content.split(delimiter)
    
    # Remove any empty strings that may result from consecutive delimiters
    elements = [element for element in elements if element.strip()]
    
    parts_list = []
    print("Elements extracted and stored in the list:")
    for i, element in enumerate(elements):
        #print(f"Element {i+1}: {element}")  # Print the first 50 characters of each element for preview
        part1, part2 = split_element(element)
        parts_list.append((part1, part2))


    delete_file(new_file)
    return parts_list

  except FileNotFoundError:
    print("The file named 'f77.semc.copy' does not exist.")
  except Exception as e:
    print(f"An error occurred: {e}")


####################### MAIN ######################################################
# Source file name
git_file = make_file_copy(".", "f77.semc", "f77.semc.git")
svn_file = make_file_copy("../f77", "f77.semc", "f77.semc.svn")

data_git = split_file_to_elements(git_file)
data_svn = split_file_to_elements(svn_file)


# File to write discrepancies
output_file = 'discrepancies.txt'

# Open the file in write mode
with open(output_file, 'w') as file:
    # Iterate over each element in data_git
    for part1_git, part2_git in data_git:
        # Find and compare with each element in data_svn
        found = False
        for part1_svn, part2_svn in data_svn:
            # Check if part1 matches
            if part1_git == part1_svn:
                found = True
                # Compare part2 if part1 matches
                if part2_git != part2_svn:
                    # Write to file if they are not equal
                    file.write(f"Mismatch found for {part1_git}:\nGit: {part2_git}\nSVN: {part2_svn}\n\n")
                break  # Stop searching once a match is found
        # If no matching part1 is found in data_svn
        if not found:
            file.write(f"No matching part1 found in SVN for {part1_git}\n\n")

print(f"Comparison complete. Discrepancies (if any) written to {output_file}.")
