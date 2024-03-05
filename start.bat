if exist public (
    hexo clean
    hexo g
) else (
    echo "Public directory already exists. Skipping hexo clean."
)
hexo s