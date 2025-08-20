FROM ghcr.io/puppeteer/puppeteer:24.2.1

ENV NODE_ENV=production \
    PUPPETEER_SKIP_DOWNLOAD=true

WORKDIR /app

COPY --chown=pptruser:pptruser package*.json ./
USER pptruser
RUN npm ci --omit=dev

COPY --chown=pptruser:pptruser . .

EXPOSE 3000

CMD ["node", "server.js"]
